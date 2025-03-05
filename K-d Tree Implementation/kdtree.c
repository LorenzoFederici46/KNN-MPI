#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#include <float.h>
#include <string.h>


typedef struct {
    double x, y, z;
    int original_index;
} Point3D;

// Prima era DistanceIdx, un pò per variare
typedef struct {
    double distance;
    int index;
} NearestNeighbor;

// NEW: salviamo le info dei nodi
// il punto di riferimento, i riferimento ai sottoalberi destro e sinistro
// la sua profondità
typedef struct KDNode {
    Point3D point;
    struct KDNode *left;
    struct KDNode *right;
    int depth;
} KDNode;

// NEW: Funzione di confronto per ogni tipo di asse 
// Poiché a ogni iterazione l'asse cambia
int compareX(const void *a, const void *b) {
    Point3D *p1 = (Point3D *)a;
    Point3D *p2 = (Point3D *)b;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

int compareY(const void *a, const void *b) {
    Point3D *p1 = (Point3D *)a;
    Point3D *p2 = (Point3D *)b;
    return (p1->y > p2->y) - (p1->y < p2->y);
}

int compareZ(const void *a, const void *b) {
    Point3D *p1 = (Point3D *)a;
    Point3D *p2 = (Point3D *)b;
    return (p1->z > p2->z) - (p1->z < p2->z);
}

// Questa la uso nel qsort per ordinare i vicini
int compareNeighbors(const void *a, const void *b) {
    NearestNeighbor *n1 = (NearestNeighbor *)a;
    NearestNeighbor *n2 = (NearestNeighbor *)b;
    return (n1->distance > n2->distance) - (n1->distance < n2->distance);
}

// Calcolo della distanza euclideana
double calculateDistance(Point3D p1, Point3D p2) {
    return sqrt(pow(p2.x - p1.x, 2) + 
                pow(p2.y - p1.y, 2) + 
                pow(p2.z - p1.z, 2));
}

// NEW: Il KD-Tree viene costruito in modo ricorsivo
KDNode* buildKDTree(Point3D *points, int start, int end, int depth) {
    if (start >= end) return NULL;

    // Seleziono l'asse corrente in base alla profondità
    int axis = depth % 3;
    
    // Ordinamento dei punti in base all'asse corrente (0 -> x) (1 -> y) (2 -> z)
    switch (axis) {
        case 0: qsort(points + start, end - start, sizeof(Point3D), compareX); break;
        case 1: qsort(points + start, end - start, sizeof(Point3D), compareY); break;
        case 2: qsort(points + start, end - start, sizeof(Point3D), compareZ); break;
    }

    // Trovo il punto mediano che diventa la radice corrente
    int mid = start + (end - start) / 2;

    // Creo il nodo e lo inserisco nell'albero
    KDNode *node = (KDNode *)malloc(sizeof(KDNode));
    node->point = points[mid];
    node->depth = depth;

    // In modo ricorsivo richiamo la funzione in modo da generare i sottoalberi 
    node->left = buildKDTree(points, start, mid, depth + 1);
    node->right = buildKDTree(points, mid + 1, end, depth + 1);

    return node;
}

// Uno dei tanti clean
void freeKDTree(KDNode *node) {
    if (node == NULL) return;
    freeKDTree(node->left);
    freeKDTree(node->right);
    free(node);
}

// Trovo i k piu' vicini per un punto
void findKNearestNeighbors(KDNode *root, Point3D target, int k, 
                            int *neighbors, double *distances) {
    // Questa parte è come prima
    NearestNeighbor *nearestNeighbors = (NearestNeighbor *)malloc(k * sizeof(NearestNeighbor));
    for (int i = 0; i < k; i++) {
        nearestNeighbors[i].distance = DBL_MAX;
        nearestNeighbors[i].index = -1;
    }

    // Per migliorare le performance, stavolta viene fatto in modo ricorsivo
    void searchKNN(KDNode *node) {
        if (node == NULL) return;

        // Calcolo la distanza del target al punto corrente
        double dist = calculateDistance(target, node->point);
        
        // Se il punto corrente è più vicino, ri ordino i vicini
        if (dist < nearestNeighbors[k-1].distance) {
            nearestNeighbors[k-1].distance = dist;
            nearestNeighbors[k-1].index = node->point.original_index;
            
            qsort(nearestNeighbors, k, sizeof(NearestNeighbor), compareNeighbors);
        }

        // Determine axis for current node
        int axis = node->depth % 3;
        double axisDiff = 0.0;
        double axisDist = 0.0;
        
        // Choose splitting strategy based on axis
        switch (axis) {
            case 0: 
                axisDiff = target.x - node->point.x; 
                axisDist = fabs(axisDiff);
                break;
            case 1: 
                axisDiff = target.y - node->point.y; 
                axisDist = fabs(axisDiff);
                break;
            case 2: 
                axisDiff = target.z - node->point.z; 
                axisDist = fabs(axisDiff);
                break;
        }

        // Determine subtrees to search
        KDNode *nearerSubtree = (axisDiff < 0) ? node->left : node->right;
        KDNode *furtherSubtree = (axisDiff < 0) ? node->right : node->left;

        // Recursively search nearer subtree first
        searchKNN(nearerSubtree);

        // Check if we need to search further subtree
        if (axisDist < nearestNeighbors[k-1].distance) {
            searchKNN(furtherSubtree);
        }
    }

    // Start recursive search
    searchKNN(root);

    // Copy results back to output arrays
    for (int i = 0; i < k; i++) {
        neighbors[i] = nearestNeighbors[i].index;
        distances[i] = nearestNeighbors[i].distance;
    }

    free(nearestNeighbors);
}

// Generate random points
void generatePoints(Point3D *points, int n, int start_idx) {
    srand(time(NULL) + start_idx);
    for (int i = 0; i < n; i++) {
        points[i].x = (double)rand() / RAND_MAX * 100.0;
        points[i].y = (double)rand() / RAND_MAX * 100.0;
        points[i].z = (double)rand() / RAND_MAX * 100.0;
        points[i].original_index = start_idx + i;
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int n = 1000;
    int k_min = 5, k_max = 20, k_step = 5;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    
    int local_n = n / size;
    int remainder = n % size;
    
    if (rank < remainder) {
        local_n++;
    }
    
    // Calcolo dell'indice di inizio di ogni processo
    int start_idx = 0;
    for (int i = 0; i < rank; i++) {
        start_idx += (i < remainder) ? (n / size + 1) : (n / size);
    }
    
    MPI_Datatype point_type;
    MPI_Datatype types[4] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
    int blocklengths[4] = {1, 1, 1, 1};
    MPI_Aint offsets[4];
    
    offsets[0] = offsetof(Point3D, x);
    offsets[1] = offsetof(Point3D, y);
    offsets[2] = offsetof(Point3D, z);
    offsets[3] = offsetof(Point3D, original_index);
    
    MPI_Type_create_struct(4, blocklengths, offsets, types, &point_type);
    MPI_Type_commit(&point_type);
    
    // Generazione parallela dei punti
    Point3D *local_points = (Point3D *)malloc(local_n * sizeof(Point3D));
    generatePoints(local_points, local_n, start_idx);
    
    // Gather point counts and collect points on master
    int *recvcounts = NULL;
    int *displs = NULL;
    Point3D *all_points = NULL;
    
    if (rank == 0) {
        // Allocate arrays for gathering point counts and displacements
        recvcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));
        
        // Collect point counts from all processes
        MPI_Gather(&local_n, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        // Compute displacements
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i-1] + recvcounts[i-1];
        }
        
        // Allocate space for full dataset
        all_points = (Point3D *)malloc(n * sizeof(Point3D));
    } else {
        // Other processes just gather their point counts
        MPI_Gather(&local_n, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    // Gather all points to master process
    MPI_Gatherv(local_points, local_n, point_type, 
                all_points, recvcounts, displs, point_type, 0, MPI_COMM_WORLD);
    
    // Build full KD-Tree on master
    KDNode *global_kdTree = NULL;
    if (rank == 0) {
        global_kdTree = buildKDTree(all_points, 0, n, 0);
    }
    
    // Prepare for distributing points to processes
    int *sendcounts = NULL;
    int *senddispls = NULL;
    Point3D *local_dataset = NULL;
    int local_dataset_size;
    
    if (rank == 0) {
        // Compute local dataset sizes and displacements
        sendcounts = (int *)malloc(size * sizeof(int));
        senddispls = (int *)malloc(size * sizeof(int));
        
        // Distribute points somewhat evenly
        local_dataset_size = n;
        int base_size = local_dataset_size / size;
        int extra = local_dataset_size % size;
        
        for (int i = 0; i < size; i++) {
            sendcounts[i] = base_size + (i < extra ? 1 : 0);
            senddispls[i] = (i > 0 ? senddispls[i-1] + sendcounts[i-1] : 0);
        }
    }
    
    // Broadcast distribution information
    MPI_Bcast(&local_dataset_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Allocate local dataset
    local_dataset = (Point3D *)malloc(local_dataset_size * sizeof(Point3D));
    
    // Scatter points to processes
    if (rank == 0) {
        // Scatter the points to all processes
        MPI_Scatterv(all_points, sendcounts, senddispls, point_type,
                     local_dataset, local_dataset_size, point_type, 
                     0, MPI_COMM_WORLD);
    } else {
        // Receive points
        MPI_Scatterv(NULL, NULL, NULL, point_type,
                     local_dataset, local_dataset_size, point_type, 
                     0, MPI_COMM_WORLD);
    }
    
    // Perform K-NN search for different k values
    for (int k = k_min; k <= k_max; k += k_step) {
        // Allocate results for local points
        int **knn_results = (int **)malloc(local_n * sizeof(int *));
        double **distances = (double **)malloc(local_n * sizeof(double *));
        
        for (int i = 0; i < local_n; i++) {
            knn_results[i] = (int *)malloc(k * sizeof(int));
            distances[i] = (double *)malloc(k * sizeof(double));
        }
        
        // Build local KD-Tree
        KDNode *local_kdTree = buildKDTree(local_dataset, 0, local_dataset_size, 0);
        
        // Perform KNN search for local points
        for (int i = 0; i < local_n; i++) {
            findKNearestNeighbors(local_kdTree, local_points[i], k, 
                                  knn_results[i], distances[i]);
            
            // Print local results
            printf("Point %d nearest neighbors: ", local_points[i].original_index);
            for (int j = 0; j < k; j++) {
                printf("%d ", knn_results[i][j]);
            }
            printf("\n");
        }
        
        // Clean up
        freeKDTree(local_kdTree);
        
        for (int i = 0; i < local_n; i++) {
            free(knn_results[i]);
            free(distances[i]);
        }
        free(knn_results);
        free(distances);
    }
    
    // Free resources
    free(local_dataset);
    free(local_points);
    
    if (rank == 0) {
        freeKDTree(global_kdTree);
        free(all_points);
        free(recvcounts);
        free(displs);
        free(sendcounts);
        free(senddispls);
    }
    
    MPI_Type_free(&point_type);
    MPI_Finalize();
    
    return 0;
}
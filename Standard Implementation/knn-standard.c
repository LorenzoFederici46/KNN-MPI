#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#include <float.h>

typedef struct {
    double x, y, z;
    int original_index;
} Point3D;


typedef struct {
    double distance;
    int index;
} DistanceIdx;

void generatePoints(Point3D *points, int n, int start_idx) {
    for (int i = 0; i < n; i++) {
        points[i].x = (double)rand() / RAND_MAX * 100.0;
        points[i].y = (double)rand() / RAND_MAX * 100.0;
        points[i].z = (double)rand() / RAND_MAX * 100.0;
        points[i].original_index = start_idx + i;
    }
}

// Distanza euclideana
double calculateDistance(Point3D p1, Point3D p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
}

// Funzione di confronto per il qsort
int compareDistances(const void *a, const void *b) {
    DistanceIdx *d1 = (DistanceIdx *)a;
    DistanceIdx *d2 = (DistanceIdx *)b;
    if (d1->distance < d2->distance) return -1;
    if (d1->distance > d2->distance) return 1;
    return 0;
}

// Trovo i k piu' vicini per un punto
void findKNN(Point3D target, Point3D *points, int n, int k, int *neighbors) {
    DistanceIdx *distances = (DistanceIdx *)malloc(n * sizeof(DistanceIdx));
    
    // Calcolo la distanza dal punto target al set di punti di riferimento
    for (int i = 0; i < n; i++) {
        distances[i].distance = calculateDistance(target, points[i]);
        distances[i].index = points[i].original_index;
    }
    
    // Ordino le distanze appena ottenute
    qsort(distances, n, sizeof(DistanceIdx), compareDistances);
    
    // Quindi prendo solo i primi k che mi interessano
    for (int i = 0; i < k && i < n; i++) {
        neighbors[i] = distances[i].index;
    }
    
    free(distances);
}

int main(int argc, char *argv[]) {
    int rank, size;
    int n = 1000; // Numero di default di punti, andrà inserito da tastiera
    int k_min = 5, k_max = 20, k_step = 5;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Parsing del numero dei punti inserito da tastiera ( se presente )
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    
    // Seed randomico per la generazione dei punti, ogni processo ne avrà uno differente
    srand(time(NULL) + rank);
    
    // Calcolo del numero di punti che ogni processo andrà a generare
    int local_n = n / size;
    int remainder = n % size;
    
    // Check in caso la size dei punti non è divisibile tra i processi
    if (rank < remainder) {
        local_n++;
    }
    
    // Indice di inizio di ogni processo
    int start_idx = 0;
    for (int i = 0; i < rank; i++) {
        start_idx += (i < remainder) ? (n / size + 1) : (n / size);
    }
    
    // Alloco la memoria e genero i punti
    Point3D *local_points = (Point3D *)malloc(local_n * sizeof(Point3D));  
    generatePoints(local_points, local_n, start_idx);
    
    // Creo un  MPI datatype per la struct relativa ai punti
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
    
    
    int *recvcounts = NULL;     // Info riguardanti il numero di punti che ogni processo è andato a generare
    int *displs = NULL;         // Offset per il corretto posizionamento dei dati
    
    if (rank == 0) {
        recvcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));
    }
    
    // Collezione delle info da parte dei processi 
    MPI_Gather(&local_n, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
/*  Calcolo del posizionamento dei blocchi per la chiamata Gather
    Se p1 invia 10, p2 invia 5, p3 invia 6, il posizionamento dei blocchi sarà displs = [0, 10, 15, 21]
    displs = [0, (0+10) = 10, (10+5) = 15, (15+6) = 21]
*/ 
    if (rank == 0) {
        displs[0] = 0; // L'offset iniziale è 0
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i-1] + recvcounts[i-1];
        }
    }
    
    // Il master colleziona i punti e crea il dataset intero
    Point3D *all_points = NULL;
    if (rank == 0) {
        all_points = (Point3D *)malloc(n * sizeof(Point3D));
    }
    
//  MPI_Gatherv(sendbuffer, sendCount, sendType,   outBuffer,  outCount, placement, outType, root, communicator )        
    MPI_Gatherv(local_points, local_n, point_type, all_points, recvcounts, displs, point_type, 0, MPI_COMM_WORLD);
    
    // Per ogni valore di k
    for (int k = k_min; k <= k_max; k += k_step) {
        // Alloco la memoria per i vicini 
        int **knn_results = (int **)malloc(local_n * sizeof(int *));
        for (int i = 0; i < local_n; i++) {
            knn_results[i] = (int *)malloc(k * sizeof(int));
        }
        
        if (rank == 0) {   /*  Operazioni del master    */
            // Il master si occupa della sua porzione dei punti
            for (int i = 0; i < local_n; i++) {
                findKNN(local_points[i], all_points, n, k, knn_results[i]);
            }
            
            // Il resto viene distribuito tra i processi
            for (int p = 1; p < size; p++) {
                int worker_points_start = displs[p];
                int worker_n = recvcounts[p];
                // Invio la porzione del dataset globale che va dall'offset di partenza fino al numero di elementi generati
                MPI_Send(&all_points[worker_points_start], worker_n, point_type, p, 0, MPI_COMM_WORLD);
                MPI_Send(all_points, n, point_type, p, 1, MPI_COMM_WORLD);
                
                // Prendo i risultati dei processi 
                for (int i = 0; i < worker_n; i++) {
                    int *neighbor_indices = (int *)malloc(k * sizeof(int));
                    MPI_Recv(neighbor_indices, k, MPI_INT, p, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
                    // Stampo i risultati
                    printf("Point %d nearest neighbors: ", worker_points_start + i);
                    for (int j = 0; j < k; j++) {
                        printf("%d ", neighbor_indices[j]);
                    }
                    printf("\n");
                    
                    free(neighbor_indices);
                }
            }
            
            // Anche il master stampa i suoi risultati
            for (int i = 0; i < local_n; i++) {
                printf("Point %d nearest neighbors: ", i);
                for (int j = 0; j < k; j++) {
                    printf("%d ", knn_results[i][j]);
                }
                printf("\n");
            }
        } else {
                        /*  Operazioni dei processi    */
            Point3D *worker_points = (Point3D *)malloc(local_n * sizeof(Point3D));
            Point3D *all_points_copy = (Point3D *)malloc(n * sizeof(Point3D));
            
            // I processi ricevono una parte dei punti e la copia del dataset completo
            MPI_Recv(worker_points, local_n, point_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(all_points_copy, n, point_type, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Calcolo i k più vicini per ogni punto
            for (int i = 0; i < local_n; i++) {
                findKNN(worker_points[i], all_points_copy, n, k, knn_results[i]);
                
                // Invio i risultati al processo master
                MPI_Send(knn_results[i], k, MPI_INT, 0, 2, MPI_COMM_WORLD);
            }
            
            free(worker_points);
            free(all_points_copy);
        }
        
        // Deallocazione e pulizia finale
    for (int i = 0; i < local_n; i++) {
        free(knn_results[i]);
    }
        free(knn_results);
    }
 
    MPI_Type_free(&point_type);
    free(local_points);
    
    if (rank == 0) {
        free(all_points);
        free(recvcounts);
        free(displs);
    }
    
    MPI_Finalize();
    return 0;
}
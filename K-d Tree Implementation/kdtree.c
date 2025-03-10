#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <time.h>
#include <float.h>
#include <string.h>
#include "util.h"


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
    
    /* Calcolo dell'indice di inizio di ogni processo */
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
    
    /* Generazione parallela dei punti */ 
    Point3D *local_points = (Point3D *)malloc(local_n * sizeof(Point3D));
    generatePoints(local_points, local_n, start_idx);
    
    int *recvcounts = NULL;
    int *displs = NULL;
    Point3D *all_points = NULL;
    
    if (rank == 0) {
        recvcounts = (int *)malloc(size * sizeof(int));
        displs = (int *)malloc(size * sizeof(int));
        
        /* Il master riceve il numero di punti generati da ogni processo */ 
        MPI_Gather(&local_n, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        /* Organizzazione dei blocchi */ 
        displs[0] = 0;
        for (int i = 1; i < size; i++) {
            displs[i] = displs[i-1] + recvcounts[i-1];
        }
        
        /* Alloco lo spazio per il dataset intero */ 
        all_points = (Point3D *)malloc(n * sizeof(Point3D));
    } else {
        /* Anche gli altri processi inviano il loro numero di punti, ma non salvano nulla */ 
        MPI_Gather(&local_n, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    /* Tutti i processi quindi inviano i punti generati per formare l'intero dataset */ 
    MPI_Gatherv(local_points, local_n, point_type, 
                all_points, recvcounts, displs, point_type, 0, MPI_COMM_WORLD);
    
    /* Il master quindi costruisce il KD tree del dataset */  
    KDNode *global_kdTree = NULL;
    if (rank == 0) {
        global_kdTree = buildKDTree(all_points, 0, n, 0);
    }
    
    /* Preparazione per la distribuzione delle porzioni di punti ai processi */ 
    int *sendcounts = NULL;
    int *senddispls = NULL;
    Point3D *local_dataset = NULL;
    int local_dataset_size;
    
    if (rank == 0) {
        sendcounts = (int *)malloc(size * sizeof(int));
        senddispls = (int *)malloc(size * sizeof(int));
        
        /* Calcolo della dimensione dei punti locali */ 
        local_dataset_size = n;
        int base_size = local_dataset_size / size;
        int extra = local_dataset_size % size;
        
        for (int i = 0; i < size; i++) {
            /* Se il numero dei punti non è divisibile, un processo ne riceve uno in più */ 
            sendcounts[i] = base_size + (i < extra ? 1 : 0);
            senddispls[i] = (i > 0 ? senddispls[i-1] + sendcounts[i-1] : 0);
        }
    }
    
    /* Distribuzione della quantità di punti di cui ogni processo si occupa */ 
    MPI_Bcast(&local_dataset_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    /* Alloco i vari dataset locali di ogni processo */ 
    local_dataset = (Point3D *)malloc(local_dataset_size * sizeof(Point3D));
    
    if (rank == 0) {
        /* Il master invia ai processi le porzioni di punti */ 
        MPI_Scatterv(all_points, sendcounts, senddispls, point_type,
                     local_dataset, local_dataset_size, point_type, 
                     0, MPI_COMM_WORLD);
    } else {
        /* I processi ricevono i punti */ 
        MPI_Scatterv(NULL, NULL, NULL, point_type,
                     local_dataset, local_dataset_size, point_type, 
                     0, MPI_COMM_WORLD);
    }
    
    /* Come prima, i KNN vengono calcolati da 5 a 20, con uno step di 5 */ 
    for (int k = k_min; k <= k_max; k += k_step) {
        /* Alloco la memoria per i risultati */ 
        int **knn_results = (int **)malloc(local_n * sizeof(int *));
        double **distances = (double **)malloc(local_n * sizeof(double *));
        
        for (int i = 0; i < local_n; i++) {
            knn_results[i] = (int *)malloc(k * sizeof(int));
            distances[i] = (double *)malloc(k * sizeof(double));
        }
        
        /* Costruzione dei KD-Tree locali */ 
        KDNode *local_kdTree = buildKDTree(local_dataset, 0, local_dataset_size, 0);
        
        /* Ricerca dei KNN */ 
        for (int i = 0; i < local_n; i++) {
            findKNearestNeighbors(local_kdTree, local_points[i], k, knn_results[i], distances[i]);
            /* Print dei risultati */ 
            printf("Point %d nearest neighbors: ", local_points[i].original_index);
            for (int j = 0; j < k; j++) {
                printf("%d ", knn_results[i][j]);
            }
            printf("\n");
        }
        
        /* Ennesimo clean up */ 
        freeKDTree(local_kdTree);
        
        for (int i = 0; i < local_n; i++) {
            free(knn_results[i]);
            free(distances[i]);
        }
        free(knn_results);
        free(distances);
    }
    
    /* Giga enormico clean up */
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
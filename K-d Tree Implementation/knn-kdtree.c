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



    /* Esecuzione */




    MPI_Finalize();
    return 0;
}
    //      mpicc -o kd knn-standard.c -lm
    //      mpirun -np 4 --oversubscribe ./kd 1000
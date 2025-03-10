#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

typedef struct {
    double x, y, z;
    int index;
} Point3D;

typedef struct {
    double distance;
    int index;
} DistanceIdx;

void generatePoints(Point3D *points, int n) {
    for (int i = 0; i < n; i++) {
        points[i].x = (double)rand() / RAND_MAX * 100.0;
        points[i].y = (double)rand() / RAND_MAX * 100.0;
        points[i].z = (double)rand() / RAND_MAX * 100.0;
        points[i].index = i;
    }
}

double calculateDistance(Point3D p1, Point3D p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
}

int compareDistances(const void *a, const void *b) {
    DistanceIdx *d1 = (DistanceIdx *)a;
    DistanceIdx *d2 = (DistanceIdx *)b;
    if (d1->distance < d2->distance) return -1;
    if (d1->distance > d2->distance) return 1;
    return 0;
}

void findKNN(Point3D *points, int n, int pointIdx, int k, int *neighbors) {
    DistanceIdx *distances = (DistanceIdx *)malloc(n * sizeof(DistanceIdx));
    
    for (int i = 0; i < n; i++) {
        if (i == pointIdx) {
            distances[i].distance = DBL_MAX;
        } else {
            distances[i].distance = calculateDistance(points[pointIdx], points[i]);
        }
        distances[i].index = i;
    }
    
    qsort(distances, n, sizeof(DistanceIdx), compareDistances);
    for (int i = 0; i < k; i++) {
        neighbors[i] = distances[i].index;
    }
    
    free(distances);
}

int main(int argc, char *argv[]) {
    int n = 1000; 
    int k_min = 5, k_max = 20, k_step = 5;
    
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    
    srand(time(NULL));
    
    Point3D *points = (Point3D *)malloc(n * sizeof(Point3D));
    generatePoints(points, n);
    
    for (int k = k_min; k <= k_max; k += k_step) {        
        int **knn_results = (int **)malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++) {
            knn_results[i] = (int *)malloc(k * sizeof(int));
        }
        
        for (int i = 0; i < n; i++) {
            findKNN(points, n, i, k, knn_results[i]);
        }
        
        /* Stampa dei risultati ( Non tutti, solo alcuni giusto per dimostrazione ) */ 
        printf("Results for k = %d (showing first 5 points):\n", k);
        int display_count = (n < 5) ? n : 5;
        for (int i = 0; i < display_count; i++) {
            printf("Point %d (%.2f, %.2f, %.2f) nearest neighbors: ", i, points[i].x, points[i].y, points[i].z);
                   
            for (int j = 0; j < k; j++) {
                printf("%d ", knn_results[i][j]);
            }
            printf("\n");
        }
        
        for (int i = 0; i < n; i++) {
            free(knn_results[i]);
        }
        free(knn_results);
    }
    
    free(points);
    return 0;
}
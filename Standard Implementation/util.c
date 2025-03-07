#include <stdio.h>
#include "util.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <string.h>

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
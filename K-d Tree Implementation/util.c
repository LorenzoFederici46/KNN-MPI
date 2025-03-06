#include <stdio.h>
#include "util.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <string.h>

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

int compareNeighbors(const void *a, const void *b) {
    NearestNeighbor *n1 = (NearestNeighbor *)a;
    NearestNeighbor *n2 = (NearestNeighbor *)b;
    return (n1->distance > n2->distance) - (n1->distance < n2->distance);
}

double calculateDistance(Point3D p1, Point3D p2) {
    return sqrt(pow(p2.x - p1.x, 2) + 
                pow(p2.y - p1.y, 2) + 
                pow(p2.z - p1.z, 2));
}

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

        // Determino il prossimo sotto-albero su cui fare la ricerca
        KDNode *nearerSubtree = (axisDiff < 0) ? node->left : node->right;
        KDNode *furtherSubtree = (axisDiff < 0) ? node->right : node->left;

        // Mi sposto nel sotto-albero e richiamo la funzione in modo ricorsivo
        searchKNN(nearerSubtree);

        // Check in caso esistono altri sotto-alberi
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

void generatePoints(Point3D *points, int n, int start_idx) {
    srand(time(NULL) + start_idx);
    for (int i = 0; i < n; i++) {
        points[i].x = (double)rand() / RAND_MAX * 100.0;
        points[i].y = (double)rand() / RAND_MAX * 100.0;
        points[i].z = (double)rand() / RAND_MAX * 100.0;
        points[i].original_index = start_idx + i;
    }
}

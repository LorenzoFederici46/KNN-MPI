#ifndef UTIL_H
#define UTIL_H

typedef struct {
    double x, y, z;
    int original_index;
} Point3D;

/* Prima era DistanceIdx, un pò per variare */
typedef struct {
    double distance;
    int index;
} NearestNeighbor;

/** @brief: Salviamo le info dei nodi
 *  il punto di riferimento, i riferimento ai sottoalberi destro e sinistro
 *  la sua profondità
 */
typedef struct KDNode {
    Point3D point;
    struct KDNode *left;
    struct KDNode *right;
    int depth;
} KDNode;

/**
 * @brief Confronta due punti in base alla coordinata X.
 *
 * Questa funzione viene utilizzata per ordinare un array di punti 3D 
 * in base alla loro coordinata X. 
 *
 * @param a Puntatore al primo punto.
 * @param b Puntatore al secondo punto.
 * @return Un valore negativo se il primo punto ha una X minore del secondo, 
 *         un valore positivo se è maggiore, e 0 se sono uguali.
 */
int compareX(const void *a, const void *b);
int compareY(const void *a, const void *b);
int compareZ(const void *a, const void *b);

/**
 * @brief Confronta due vicini più prossimi in base alla distanza.
 *
 * Questa funzione viene utilizzata da qsort per ordinare un array 
 * di vicini in ordine crescente di distanza dal punto di riferimento.
 *
 * @param a Puntatore al primo vicino.
 * @param b Puntatore al secondo vicino.
 * @return Un valore negativo se il primo vicino è più vicino del secondo,
 *         un valore positivo se è più lontano, e 0 se hanno la stessa distanza.
 */
int compareNeighbors(const void *a, const void *b);

/**
 * @brief Calcola la distanza euclidea tra due punti 3D.
 *
 * Questa funzione calcola la distanza tra due punti nello spazio tridimensionale
 * utilizzando la formula della distanza euclidea.
 *
 * @param p1 Il primo punto nello spazio 3D.
 * @param p2 Il secondo punto nello spazio 3D.
 * @return La distanza euclidea tra p1 e p2.
 */
double calculateDistance(Point3D p1, Point3D p2);

/**
 * @brief Costruisce un KD-Tree ricorsivamente a partire da un array di punti 3D.
 *
 * La funzione costruisce un albero KD partendo da un array di punti 3D. I punti vengono
 * ordinati in base all'asse corrente (x, y o z) determinato dal livello di profondità,
 * e il punto mediano diventa la radice dell'albero. La funzione è ricorsiva e costruisce
 * il sottoalbero sinistro e destro chiamando la funzione su sottoinsiemi di punti.
 *
 * @param points Array di punti 3D.
 * @param start Indice di inizio dell'intervallo di punti da considerare.
 * @param end Indice di fine dell'intervallo di punti da considerare.
 * @param depth Profondità corrente nell'albero, usata per determinare l'asse su cui ordinare.
 * 
 * @return Puntatore alla radice dell'albero KD creato.
 * 
 * @note L'array di punti deve essere allocato e inizializzato prima della chiamata.
 */
KDNode* buildKDTree(Point3D *points, int start, int end, int depth);


/**
 * @brief Libera la memoria occupata da un albero KD in modo ricorsivo.
 * @param node Puntatore alla radice dell'albero KD (o sottoalbero) da liberare.
 * @note Questa funzione non restituisce alcun valore e assume che i nodi siano stati allocati dinamicamente.
 */
void freeKDTree(KDNode *node);

/**
 * @brief Trova i k vicini più prossimi per un punto target in un albero KD.
 *
 * La funzione esegue una ricerca ricorsiva nel KD-Tree per trovare i k punti più vicini al punto target.
 * Viene utilizzata una struttura di dati ausiliaria per tenere traccia dei k vicini più prossimi
 * e delle relative distanze. La ricerca è ottimizzata per visitare prima il sottoalbero più vicino e
 * successivamente il sottoalbero più lontano solo se necessario.
 *
 * @param root Puntatore alla radice dell'albero KD.
 * @param target Il punto di riferimento per cui trovare i vicini più prossimi.
 * @param k Numero di vicini più prossimi da trovare.
 * @param neighbors Array in cui verranno memorizzati gli indici dei k vicini più prossimi.
 * @param distances Array in cui verranno memorizzate le distanze corrispondenti ai k vicini più prossimi.
 * 
 * @note La funzione aggiorna gli array `neighbors` e `distances` con gli indici e le distanze
 *       dei k vicini più prossimi trovati. Gli array devono essere già allocati prima della chiamata.
 */
void findKNearestNeighbors(KDNode *root, Point3D target, int k,int *neighbors, double *distances);

/**
 * @brief Genera punti casuali in uno spazio 3D e li memorizza in un array.
 *
 * La funzione genera `n` punti casuali con coordinate x, y, z comprese tra 0 e 100. 
 * Ogni punto viene memorizzato in un array di strutture `Point3D` e viene assegnato un indice 
 * univoco (`original_index`) che parte da `start_idx` e aumenta progressivamente.
 *
 * @param points Array di punti 3D in cui memorizzare i punti generati.
 * @param n Numero di punti casuali da generare.
 * @param start_idx Indice iniziale da cui partire per assegnare `original_index` ai punti generati.
 * 
 * @note La funzione utilizza la funzione `rand()` per generare le coordinate casuali, 
 *       quindi il seme del generatore di numeri casuali è impostato usando `srand(time(NULL) + start_idx)` 
 *       per garantire la variabilità dei risultati su chiamate successive.
 */
void generatePoints(Point3D *points, int n, int start_idx);


#endif

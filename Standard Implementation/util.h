#ifndef UTIL_H
#define UTIL_H

typedef struct {
    double x, y, z;
    int original_index;
} Point3D;

/* Struttura per salvare le distanze */
typedef struct {
    double distance;
    int index;
} DistanceIdx;

/**
 * @brief Confronta due vicini più prossimi in base alla distanza.
 *
 * Questa funzione viene utilizzata da qsort per ordinare un array 
 * di vicini in ordine crescente di distanza dal punto di riferimento.
 *
 * @param a Puntatore al primo punto.
 * @param b Puntatore al secondo punto.
 * @return Un valore negativo se il primo punto è più vicino del punto,
 *         un valore positivo se è più lontano, e 0 se hanno la stessa distanza.
 */
int compareDistances(const void *a, const void *b) ;

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

/**
 * @brief Trova i k vicini più prossimi di un punto target.
 *
 * Questa funzione calcola la distanza euclidea tra un punto target e un insieme
 * di punti di riferimento, ordina i risultati e restituisce gli indici dei k punti più vicini.
 *
 * @param target Punto target per cui trovare i vicini.
 * @param points Array di punti di riferimento.
 * @param n Numero totale di punti di riferimento.
 * @param k Numero di vicini da trovare.
 * @param neighbors Array di output che conterrà gli indici dei k punti più vicini.
 */
 void findKNN(Point3D target, Point3D *points, int n, int k, int *neighbors);

#endif

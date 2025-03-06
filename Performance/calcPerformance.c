#include <stdio.h>

void compute_speedup_efficiency(double times[], int num_processors[], int size) {
    double T1 = times[0]; // Tempo di esecuzione con un solo processore
    
    printf("Processors\tTime\tSpeedup\tEfficiency\n");
    for (int i = 0; i < size; i++) {
        int p = num_processors[i];
        double Tp = times[i];
        double speedup = T1 / Tp;
        double efficiency = speedup / p;
        printf("%d\t\t%.2f\t%.2f\t%.2f\n", p, Tp, speedup, efficiency);
    }
}

int main() {
    // Esempio di input
    int num_processors[] = {1, 2, 4, 8, 16};
    double times[] = {100.0, 55.0, 30.0, 18.0, 12.0}; // Esempi di tempi
    int size = sizeof(num_processors) / sizeof(num_processors[0]);

    compute_speedup_efficiency(times, num_processors, size);
    
    return 0;
}

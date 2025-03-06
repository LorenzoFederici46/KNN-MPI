#include <stdio.h>

void compute_speedup_efficiency(double times[], int num_processors[], int size) {
    double sequential = 100.0
    
    printf("Processors\tTime\tSpeedup\tEfficiency\n");

    for (int i = 0; i < size; i++) {
        int p = num_processors[i];
        double Tp = times[i];

        double speedup = sequential / Tp;
        double efficiency = speedup / p;

        printf("%d\t\t%.2f\t%.2f\t%.2f\n", p, Tp, speedup, efficiency);
    }
}

int main() {

    int num_processors[] = {2, 4, 8, 12};
    double times[] = {55.0, 30.0, 18.0, 12.0};
    int size = sizeof(num_processors) / sizeof(num_processors[0]);

    compute_speedup_efficiency(times, num_processors, size);
    return 0;
}

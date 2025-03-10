# k-NN with MPI: Comparing Sequential and Parallel Execution

This project implements the k-Nearest Neighbors (k-NN) algorithm using parallelism via MPI to demonstrate the advantages of parallel execution over sequential execution.

## Project Structure

The project is organized into four main directories:

- **kd_tree/**: Implements k-NN using a KD-Tree to accelerate neighbor searches.
- **sequential/**: Sequential implementation of k-NN without parallel optimizations.
- **knnstandard/**: Parallel implementation of k-NN using MPI, without the KD-Tree.
- **performance/**: Scripts for calculating speedup and efficiency based on execution times and the number of processors used.

Each folder contains a **Makefile** for easy compilation and execution.

## Compilation and Execution

To compile and run each implementation, navigate to the respective folder and execute:

```bash
make
```

To run the sequential version:
```bash
./knn_sequential dataset.txt k
```

To run the parallel version with MPI:
```bash
mpirun -np <num_processes> ./knn_mpi dataset.txt k
```

## Performance Evaluation

After collecting execution times, the **performance/** directory contains tools to calculate:
- **Speedup**: How much faster the parallel execution is compared to the sequential one.
- **Efficiency**: The ratio between speedup and the number of processors used.

Example analysis:
```bash
python3 analyze_performance.py times.txt
```

## Requirements

- **MPI**: Install an MPI implementation (e.g., OpenMPI or MPICH)
- **C Compiler**: GCC or equivalent
- **Python** (optional): For performance analysis

## Author
This project was developed to demonstrate the effectiveness of parallelism in the k-NN algorithm using MPI. If you have any questions or suggestions, feel free to contribute!


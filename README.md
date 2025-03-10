# k-NN with MPI: Comparing Sequential and Parallel Execution

This project implements the k-Nearest Neighbors (k-NN) algorithm using parallelism via MPI to demonstrate the advantages of parallel execution over sequential execution.

## Project Structure

The project is organized into four main directories:

- **K-d Tree Implementation/**: Implements k-NN using a KD-Tree to accelerate neighbor searches.
- **Sequential Implementation/**: Sequential implementation of k-NN without parallel optimizations.
- **Standard Implementation/**: Parallel implementation of k-NN using MPI, without the KD-Tree.
- **Performance/**: C file for calculating speedup and efficiency based on execution times and the number of processors used.

Each folder contains a **Makefile** for easy compilation and execution.

## Compilation and Execution

To compile and run each implementation, navigate to the respective folder and execute:

```bash
make
```

To run the sequential version:
```bash
make run n=<number_of_points>
```

To run the parallel version with MPI, the type of run depends on the number of processors used, check the Makefile for more security:
```bash
make <run_p> n=<number_of_points>
```

## Performance Evaluation

After collecting execution times, the **Performance/** directory contains tools to calculate:
- **Speedup**: How much faster the parallel execution is compared to the sequential one.
- **Efficiency**: The ratio between speedup and the number of processors used.
Navigate the C file, and insert in the correct array the execution times evaluated, then compile and run the file in the following way:

Compile:
```bash
make
```

Execute:
```bash
make run
```

## Requirements

- **MPI**: Install an MPI implementation (e.g., OpenMPI or MPICH)
- **C Compiler**: GCC or equivalent

## Author
This project was developed to demonstrate the effectiveness of parallelism in the k-NN algorithm using MPI. If you have any questions or suggestions, feel free to contribute!


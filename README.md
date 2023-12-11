# Matrix Multiplication Performance Analysis using Parallelism in C
This C project focuses on analyzing the execution time of multiplying two matrices using different parallelism techniques. It compares the performance between native, multiprocessing, and multithreading approaches while considering factors such as clearing cached memory before each execution and emphasizing performance on a dual-boot Ubuntu system over a virtual machine environment.

## Overview
The project aims to demonstrate the efficiency and performance gains achieved through parallelism techniques in matrix multiplication. It explores three primary approaches:

1. **Native Approach:** Traditional matrix multiplication algorithm without parallelism.
2. **Multiprocessing Approach:** Utilizing multiple processes for matrix multiplication.
3. **Multithreading Approach:** Implementing multithreading to enhance computation speed.

### Features

- Matrix Multiplication: Implements matrix multiplication algorithms for each approach.
- Performance Analysis: Measures execution time for each approach.
- Platform Emphasis: Prioritizes execution on a dual-boot Ubuntu system over a virtual machine for accurate performance evaluation.

## Getting Started

### Prerequisites
- C compiler (gcc recommended).
- Pthread libraries
- Dual-boot Ubuntu system.

### Usage
1. Ensure you're running the code on a dual-boot Ubuntu system for accurate performance metrics.
2. Run each executable separately: ./main .
3. Observe the execution times for different approaches and compare the performance.

### Considerations
- To ensure fair comparisons, **clear cached memory** before each execution:
- Running the code on a dual-boot system provides more accurate performance evaluation than using a virtual machine due to system resource allocation differences.

## Example

```plaintext

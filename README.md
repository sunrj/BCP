# CPlex

## Running Format

The source codes of our paper are in the folder "src".

The source code of each algorithm has four parameters, corresponding to the algorithm name (e.g., **OptEnum**), the dataset name (e.g., **D6**), the integer $k$ (e.g., **1**), and the size constraint &delta; (e.g., **3**).

* Running example

  Take **OptEnum** as an example, on dataset **D6** with parameters $k$ = **1** and &delta; = **3**

  > g++ -std=c++11 main.cpp graph.cpp -O3 -o OptEnum
  >
  > ./OptEnum OptEnum D6 1 3

## Datasets

We provide six datasets in the folder "sample_dataset" for testing, D1 to D6.

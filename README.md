# Cplex

## Running Format

The source codes of our paper are in the folder "src".

The source code of each algorithm has four parameters, corresponding to the algorithm name (e.g., **OptEnum**), the dataset name (e.g., **D6**), an integer $k$ (e.g., **1**), and the size constraint &delta; (e.g., **3**).

* Running example for Algorithms **VFree** and **VFree-**

  Take **VFree** as an example, on dataset **D5** with parameters $\tau_U$ = **5**, $\tau_V$ = **4** and &lambda; = **3**

  > g++ -std=c++11 RecurrenceGroup.cpp -O3 -o VFree
  >
  > ./VFree VFree D5 5 4 3


* Running example for Algorithms **FilterV**, **FilterV-**, **FilterV-FR**, **FilterV-VM** and **BK-ALG+**

  Take **FilterV** as an example, on dataset **D5** with parameters $\tau_U$ = **5**, $\tau_V$ = **4** and &lambda; = **3**

  > g++ -std=c++11 main.cpp graph.cpp -O3 -o FilterV
  >
  > ./FilterV FilterV D5 5 4 3

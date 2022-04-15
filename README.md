# Code for SIGMOD submission
========================================================================

This repository stores the source code of the proposed algorithm (LiteHST) to solve the similarity search problem.


Usage of the source code
---------------

### Environment

gcc/g++ version: 7.5.0 

Python version: 2.7.5 

### Compile the algorithm

##### Baselines: BST BKT MVPT GNAT

cd algorithm/baselines && make all

TestBST: the baseline BST in the experiments

TestBKT: the baseline BKT in the experiments

TestGNAT: the baseline GNAT in the experiments

TestMVPT: the baseline MVPT in the experiments

TestSPBPT: the baseline SPBT in the experiments

##### Our proposed algorithm: LiteHST

cd algorithm/LiteHST && make all

FastMap: the algorithm LiteHST_{FastMap} in the experiments

Mtree: the algorithm LiteHST_{Mtree} in the experiments

LiteHSTO0: the range query and kNN query by LiteHST (without the learning-based enhancement) in the experiments

LiteHST: the kNN query by LiteHST (with the learning-based enhancement) in the experiments

##### Note that you need to modify the directory of the datasets in the global.cpp before runing the experiments.

### Datasets

dataset/color: the Color dataset used in the experiments

dataset/dutch: the Dutch dataset used in the experiments

dataset/english: the English dataset used in the experiments

dataset/synthetic: the BinStr dataset used in the experiments

scripts/genSyntheticData.py: the data generator for the synthetic dataset, i.e., BinStr

scripts/genQueryWorload.py: the data generator for the range queries and kNN queries

##### Note that the scripts can be used to generate more queries and we only provide a sample.

### Run the algorithm

TestBST english: test the English dataset

TestBST dutch: test the Dutch dataset

TestBST color: test the Color dataset

TestBST synthetic: test the BirStr dataset

##### Note that TestBST can be replaced with TestGNAT, TestBKT, TestMVPT, TestSPBT, FastMap, Mtree and LiteHST.	






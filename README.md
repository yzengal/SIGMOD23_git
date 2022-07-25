# Code for SIGMOD submission
========================================================================

This repository stores the source code of the proposed algorithm (LiteHST) to solve the similarity search problem. The code for the revision has also been updated.


Usage of the source code
---------------

### Environment

gcc/g++ version: 7.5.0 

Python version: 2.7.5 

### Compile the algorithm

##### Baselines: BST, BKT, MVPT, GNAT, SPB-Tree+PGM-index

cd algorithm/baselines && make all

TestBST: the baseline BST in the experiments

TestBKT: the baseline BKT in the experiments

TestGNAT: the baseline GNAT in the experiments

TestMVPT: the baseline MVPT in the experiments

TestSPBTpgm: the baseline in-memory SPB-Tree (with the [PGM-index](https://github.com/gvinciguerra/PGM-index) enhancement) in the experiments

##### Baseline: in-memory Mtree

cd inMTREE && make all

TestMtree: the baseline [in-memory Mtree](https://github.com/erdavila/M-Tree) in the experiments

##### Baselines: DSACLT+, LC, PMTREE, external-memory Mtree, external-memory SPB-Tree

cd DSACLT && make all

TestDSA: the baseline DSACLT+ in the experiments

cd LC && make all

TestLC: the baseline LC in the experiments

cd PMTREE && make all

TestPMT: the baseline PMTree in the experiments

cd extMTREE && make all

TestMT: the baseline external-memory MTree in the experiments

cd extSPBTREE && make all

TestSPBT: the baseline external-memory SPB-Tree in the experiments

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

dataset/SIFT: the SIFT 100M dataset used in the experiments (too large, please refer to the [source](http://corpus-texmex.irisa.fr/))

scripts/genSyntheticData.py: the data generator for the synthetic dataset, i.e., BinStr

scripts/genQueryWorload.py: the data generator for the range queries and kNN queries

scripts/processSIFT.cpp: the data generator for the SIFT dataset 

##### Note that the scripts can be used to generate more queries and we only provide a sample.

### Run the algorithm for the in-memory experiments

TestBST english: test the English dataset

TestBST dutch: test the Dutch dataset

TestBST color: test the Color dataset

TestBST synthetic: test the BirStr dataset

##### Note that TestBST can be replaced with TestGNAT, TestBKT, TestMVPT, TestSPBTpgm, TestMtree, FastMap, Mtree, and LiteHST.	

### Run the algorithm for the in-memory experiments

TestLC: test the SIFT100M dataset

##### Note that TestLC can be replaced with TestDSA, TestPMT, TestMT, TestSPBT, and TestLiteHST.	






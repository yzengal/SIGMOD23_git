/*
Embedding Arbitrary Metrics By Tree Metrics: From Theory To Practice

Authors: Yuxiang Zeng, Yongxin Tong, Lei Chen
Contact: yzengal@connect.ust.hk
Affiliation: The Hong Kong University of Science and Technology, Beihang University

The MIT License (MIT)

Copyright (c) 2020 The Hong Kong University of Science and Technology, Beihang University

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#ifndef MONITOR_H
#define MONITOR_H

#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

struct program_t {
	// struct timespec real;
	// struct timespec proc;
	struct timeval real;
};

void save_time(program_t& prog);
double calc_time(const program_t& st, const program_t& ed);
int get_proc_status(int pid, const char* mark);
void watchSolution(pid_t pid, int& usedMemory, int& usedTime, int limitTime);
void watchSolutionOnce(pid_t pid, int& usedMemory);


#endif

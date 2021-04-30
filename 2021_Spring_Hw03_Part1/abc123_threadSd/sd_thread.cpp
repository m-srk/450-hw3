#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include "sd_thread.h"

using namespace std;

typedef struct thread_params {
	int id;
	int istart;
	int istop;
	double *A;
	long N;
	int numthrs;
	double mean;
	double sd;
	double min;
	double max;
} tparams;

void binWorker(tparams *tp) {
	double *A = tp->A, mean = tp->mean;

	for(long i = tp->istart; i < tp->istop; i += 1)
	{
		tp->sd += (A[i] - mean) * (A[i] - mean);
		
		if(tp->max < A[i])
		{
			tp->max = A[i];
		}
		
		if(tp->min > A[i])
		{
			tp->min = A[i];
		}
	}
}

void meanWorker(tparams *tp) {
	// perform the summation for the mean
	long N = tp->N; double sum = 0;
	for(long i = tp->istart; i < tp->istop; i += 1)
	{
		sum = sum + tp->A[i];
	}

	tp->mean = sum;
}

STDDEV_RESULT* calcSdThread(double *A, long N, int P)
{
    struct STDDEV_RESULT* res = new STDDEV_RESULT;
    
    double sd_temp, mean, min, max, sd;
    
    min = RAND_MAX;
    max = 0.0;
    sd = 0;
    sd_temp = 0;
    mean = 0;

	thread t[P];
	tparams *tp = (tparams *) malloc(P * sizeof(tparams));
	for (int i=0; i<P; i++) {
		tp[i].A = A;
		tp[i].N = N;
		tp[i].istart = i*(N/P);
		tp[i].istop = (i+1)*(N/P);
		tp[i].numthrs = P;

		t[i] = thread(meanWorker, &tp[i]);
	}

	for (int i=0; i<P; i++) {
		t[i].join();
	}

	for (int i=0; i<P; i++) {
		mean += tp[i].mean;
	}

	mean /= (double) N;

	thread newt[P];
	for (int i=0; i<P; i++) {
		tp[i].A = A;
		tp[i].N = N;
		tp[i].istart = i*(N/P);
		tp[i].istop = (i+1)*(N/P);
		tp[i].numthrs = P;
		tp[i].mean = mean;
		tp[i].sd = sd;
		tp[i].min = min;
		tp[i].max = max;

		newt[i] = thread(binWorker, &tp[i]);
	}

	for (int i=0; i<P; i++) {
		newt[i].join();
	}

	// find min and max
	for(long i = 0; i < P; i++)
	{
		sd += tp[i].sd;
		if(max < tp[i].max)
		{
			max = tp[i].max;
		}
		if(min > tp[i].min)
		{
			min = tp[i].min;
		}
	}

	sd=sqrt(sd/(double)N);
	
	// store off the values to return 
	res->mean = mean;
	res->min = min;
	res->max = max;
	res->stddev = sd;
	
    return res;
}

THRESH_RESULT *findThreshValuesThread(double *A, long N, double T, int P)
{
	THRESH_RESULT *p_tmpResult = new THRESH_RESULT;
	
	// traverse the list once to find the count of values over threshold
	long c = 0;
	for (long i=0; i < N; i++)
	{
		if (A[i] > T)
			c++;
	}
	
	// store the count and allocate an array to store the results
	p_tmpResult->li_threshCount = c;
	p_tmpResult->pli_list = new long[c];
	c = 0;
	
	// store the index locations of the values over threshold
	for (long i=0; i < N; i++){
		if (A[i] > T){
			p_tmpResult->pli_list[c] = i;
			c++;
		}
	}
	
	return p_tmpResult;
}

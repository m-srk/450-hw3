/**
 * CMPCS-450 Assignment 3
 * main.cpp
 * Warning: Do not change anything in this file
 * 
 * @author Niranjan Thirusangu, William Seisler
 * @version 1.2 02/18/2020
*/

#include <iostream>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits>
#include "sd_thread.h"
#include "calcSd.h"

using namespace std;

void get_walltime(double* wcTime);

int main(int argc, char *argv[])
{
	// some variables to track walltime for benchmark
	double wcs, wce, serial_duration, thread_duration;

	// a separate results object for each test
	STDDEV_RESULT* res_serial; 
	STDDEV_RESULT* res_thread;

	long N = 1000000000;
	int P = 10;
	double T = 999.0;

	if (argc >= 2)
	{ // 1st argument is N
		N = atoi(argv[1]);
	}
	if (argc >= 3)
	{ // 2nd argument is P
		P = atoi(argv[2]);
	}
	if (argc >= 4)
	{ // 3rd argument is T
		T = atof(argv[3]);
	}
	
	printf("N is set to: %d\n",N);
	printf("P is set to: %d\n",P);
	printf("T is set to: %0.5f\n",T);
	
	srand(time(0));
	
	double *A = new double[N];
	
	for(long i=0;i<N;i++)
	{
		A[i] = 1000.0 * (double)rand() / (double)RAND_MAX;
	}

	// benchmark serial implementation
	get_walltime(&wcs);
	res_serial = calcSdSerial(A, N);
	get_walltime(&wce);
	serial_duration = wce-wcs;
	
	// benchmark threaded implementation
	get_walltime(&wcs);
	res_thread = calcSdThread(A, N, P);
	get_walltime(&wce);
	thread_duration = wce-wcs;
	
	// some simple checks
	printf("Checking correctness of the Program \n");
	printf("Min: %f, %f, Check %s \n", res_thread->min, res_serial->min, 
			(res_thread->min==res_serial->min?"passed":"failed") );
			
	printf("Max: %f, %f, Check %s \n", res_thread->max, res_serial->max, 
			(res_thread->max==res_serial->max?"passed":"failed") );
			
	printf("Mean: %f, %f, Check %s \n", res_thread->mean, res_serial->mean, 
			(res_thread->mean==res_serial->mean?"passed":"failed") );
			
	printf("Standard Deviation: %0.9f, %0.9f, Check %s\n", res_thread->stddev, res_serial->stddev, 
			(res_thread->stddev==res_serial->stddev?"passed":"failed"));
			
	printf("STD_DEV Wall times: OMP: %f, Serial: %f\n", thread_duration, serial_duration);
			
			
	THRESH_RESULT *p_serialThreshRes;
	THRESH_RESULT *p_threadThreshRes;
			
	// benchmark serial threshold implementation
	get_walltime(&wcs);
	p_serialThreshRes = findThreshValuesSerial(A, N, T);
	get_walltime(&wce);
	serial_duration = wce-wcs;
	
	// benchmark threaded threshold implementation
	get_walltime(&wcs);
	p_threadThreshRes = findThreshValuesThread(A, N, T, P);
	get_walltime(&wce);
	thread_duration = wce-wcs;
			
	
	printf("Checking correctness of Thresholding \n");
	printf("Threshold Count: %i, %i, Check %s\n", 
			p_threadThreshRes->li_threshCount, 
			p_serialThreshRes->li_threshCount, 
			(p_threadThreshRes->li_threshCount==p_serialThreshRes->li_threshCount ? "passed":"failed"));
			
	for (long ii = 0; ii < p_serialThreshRes->li_threshCount; ii++)
	{
		if (A[p_serialThreshRes->pli_list[ii]] < T)
		{
			printf("Error! Serial Threshold list is inaccurate! Item %i failed! A[%i] = %f (%f)\n",
					ii, p_serialThreshRes->pli_list[ii], 
					A[p_serialThreshRes->pli_list[ii]], T);
			break;
		}
	}

	for (long ii = 0; ii < p_threadThreshRes->li_threshCount; ii++)
	{
		if (A[p_threadThreshRes->pli_list[ii]] < T)
		{
			printf("Error! Thread Threshold list is inaccurate! Item %i failed! A[%i] = %f (%f)\n",
					ii, p_threadThreshRes->pli_list[ii], 
					A[p_threadThreshRes->pli_list[ii]], T);
			break;
		}
	}
	
	printf("THRESH Wall times: OMP: %f, Serial: %f\n", thread_duration, serial_duration);		
			
	delete[] A;
	delete res_serial;
	delete res_thread;
	
	delete[] p_threadThreshRes->pli_list;
	delete p_threadThreshRes;

	delete[] p_serialThreshRes->pli_list;
	delete p_serialThreshRes;
	
	
	return 0;
}

void get_walltime(double* wcTime) 
{
     struct timeval tp;
     gettimeofday(&tp, NULL);
     *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}

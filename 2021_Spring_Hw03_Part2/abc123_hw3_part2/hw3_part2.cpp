/*
 ** This file is part of HW-2 for CMPSC 450 2020-21
 ** @author Sai Srikanth Mantravadi V.S
 ** @email sxm6373@psu.edu
 ** PSU ID : sxm6373
 **
 ** This file aims to optimize a given library routine that performs matrix-matrix multiplication
 ** Optimization to be performed is serial and memory access
 */

#include <iostream>
#include <math.h>

using namespace std;

// naive MMA
void matrix_mult_naive(double *A, double *B, double *C, int N)
{
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
			C[i * N + j] = 0;
			for (int k = 0; k < N; k++)
			{
				C[i * N + j] += A[i * N + k] * B[k * N + j];
			}
		}
}

// block based MMA
void matrix_mult(double *A, double *B, double *C, int N)
{
	int row, col, i, j, k;
	int b = 40;
	double sum;

	for (int p =0; p<N; p++) 
	{
		for (int q =0; q<N; q++) 
		{
			C[p*N + q] = 0.0;
		}
	} 

	for (row = 0; row < N; row += b) 
	{
		cout << "row = " << row << endl;
		for (col = 0; col < N; col += b) 
		{
			cout << "col = " << col << endl;
			for (i = 0; i < N; i++) 
			{
				cout << "i = " << i << endl;
				for (j = col; j < col+b; j++) 
				{
					printf("C[%d][%d] =\n", i, j);
					sum = C[i*N + j];
					for (k = row; k < row+b; k+=1)
					{
						cout << "k = " << k << endl;
						printf("+ A[%d][%d] * B[%d][%d]\n", i, k, k, j);
						sum += A[i*N + k] * B[k*N + j];
					}
					C[i*N + j] = sum;
				}
			}
		}

	}

	free(A); free(B);

}


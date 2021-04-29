#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

typedef struct MatThrArgs {
    int id;
    int numThreads;
} MatThrArgs_t;

void doBlockMMA(){}

void transpose (double *A, int N)
{
    double t;
    for (int i = 0; i < N; i++) {
        for (int j = i+1; j < N; j++) {
            t = A[i*N + j];
            A[i*N + j] = A[j*N + i];
            A[j*N + i] = t;
        }
    }
}

// transpose based MMA
void matrix_mult_txpose(double *A, double *B, double *C, int N)
{
    int row, col, i, j, k;
	double sum;

    // O(n^2)
	for (int p =0; p<N; p++) 
	{
		for (int q =0; q<N; q++) 
		{
			C[p*N + q] = 0.0;
		}
	}

    // should work, easy to thread
    transpose(B, N);

    for(int i=0; i<N; i++) {
        for(int j=0; j<N; j++) {
            double tmp = 0;
            for(int l=0; l<N; l++) {
                tmp += A[N*i+l] * B[N*j+l];
            }
            C[N*i + j] = tmp;
        }
    }

    // for (int i = 0; i < N; i++) {
    //     for (int j = 0; j < N; j++) {
    //         for (int k = 0; k < N; k++) {
    //             C[i*N + j] += A[i*N + k] * B[k*N + j] ;
    //         }
    //     }
    // }    

}

// block based MMA
void matrix_mult_block(double *A, double *B, double *C, int N)
{
	int row, col, i, j, k;
	int b = 2;
	double sum;
    int count;

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
					printf("\nC[%d][%d] +=\n", i, j);
					sum = C[i*N + j];
					for (k = row; k < row+b; k+=1)
					{
						// cout << "k = " << k << endl;
						printf("+ A[%d][%d] * B[%d][%d] ", i, k, k, j);
						sum += A[i*N + k] * B[k*N + j];
					}
					C[i*N + j] = sum;
                    count++;
                    cout << endl;
				}
			}
		}

	}

    cout << "Count is : " << count << endl;

}

int main () {

    int N = 4;
    double *A = new double[N*N];
    double *B = new double[N*N];
    double *C = new double[N*N];

    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            A[i*N + j] = i;
            B[i*N + j] = 0;
        }
        B[i*N + i] = 2.0;
    }

    cout << "A: " << endl;
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            cout << A[i*N + j] << ", ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "B: " << endl;
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            cout << B[i*N + j] << ", ";
        }
        cout << endl;
    }
    cout << endl;

    matrix_mult(A, B, C, N);

    cout << "C: " << endl;
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            cout << C[i*N + j] << ", ";
        }
        cout << endl;
    }
    cout << endl;

    free(A); free(B); free(C);

    return 0;
}
#include <iostream>
#include <mutex>
#include <thread>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

using namespace std;

typedef struct MatThrArgs {
    int id;
    int numThreads;
    double *A;
	double *B;
	double *C;
	int N;
    int i_start;
    int step;
	int i_stop;
} MatThrArgs_t;

void get_walltime(double* wcTime) 
{
     struct timeval tp;
     gettimeofday(&tp, NULL);
     *wcTime = (double)(tp.tv_sec + tp.tv_usec/1000000.0);
}

void printMat (double *A, int N, const char* label) {
    cout << label << endl;
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            cout << A[i*N + j] << ", ";
        }
        cout << endl;
    }
    cout << endl;
}

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

void matrix_multi(struct MatThrArgs *params) {

    for(int i = params->i_start; i < params->i_stop; i+= 5)
    {
        for(int j = 0; j < params->N; j++)
        {
	    (params->C)[params->N*(i) + j] = (params->C)[params->N*(i+1) + j] = 0;
	    (params->C)[params->N*(i+2) + j] = (params->C)[params->N*(i+3) + j] = (params->C)[params->N*(i+4) + j] = 0;
            for(int l=0; l < params->N; l++)
            {
                (params->C)[params->N*i + j] += (params->A)[params->N*i+l] * (params->B)[params->N*j+l];
                (params->C)[params->N*(i+1) + j] += (params->A)[params->N*(i+1)+l] * (params->B)[params->N*j+l];
                (params->C)[params->N*(i+2) + j] += (params->A)[params->N*(i+2)+l] * (params->B)[params->N*j+l];
                (params->C)[params->N*(i+3) + j] += (params->A)[params->N*(i+3)+l] * (params->B)[params->N*j+l];
                (params->C)[params->N*(i+4) + j] += (params->A)[params->N*(i+4)+l] * (params->B)[params->N*j+l];
            }
            // cout << "Thread: " << params->id << " , Computing C[" << i << "][" << j << "]" << endl;
        }
    }
}

void thrTxpose(struct MatThrArgs *params)
{
    int N = params->N;
    double t;
    double *A = params->A;
    
    for(int i = params->i_start; i < params->i_stop; i += params->step)
    {
        // cout << "Thread : " << params->id << " , i = " << i << endl;
        for(int j = i+1; j < N; j++)
        {
            t = A[i*N + j];
            A[i*N + j] = A[j*N + i];
            A[j*N + i] = t;
        }   
    }
}

void transposeThreaded(double *A, int N) {

    int NUMTHREADS = atoi( getenv("NUMTHREADS") );
    cout << NUMTHREADS	<< endl;
	struct MatThrArgs *params = new struct MatThrArgs[NUMTHREADS];

    for (int i = 0; i < NUMTHREADS; i++)
	{
		params[i].i_start = i;
		params[i].i_stop = N;
		params[i].id = i;
        params[i].A = A;
		params[i].N = N;
        params[i].step = NUMTHREADS;
	}

    thread t[NUMTHREADS];

    // cout << "[Txpose Threaded] Firing threads ->" << endl;

    for (int i = 0; i < NUMTHREADS; i++) {
		t[i] = thread(thrTxpose, &params[i]);
	}

    for (int i = 0; i < NUMTHREADS; i++) {
    	t[i].join();
    }

    // cout << "[Txpose Threaded] Threads joined ->" << endl;

    delete [] params;
}

// matrix_mult_opt_threaded
void matrix_mult(double *A, double *B, double *C, int N)
{
    transposeThreaded(B, N);
    // printMat(B, N, "Txpose(B):");

    int NUMTHREADS = atoi( getenv("NUMTHREADS") );

    struct MatThrArgs *params = new struct MatThrArgs[NUMTHREADS];

    for (int i = 0; i < NUMTHREADS; i++)
	{
		params[i].i_start = i * (N/NUMTHREADS);
		params[i].i_stop = (i + 1) * (N/NUMTHREADS);
		params[i].id = i;
        	params[i].A = A;
		params[i].B = B;
		params[i].C = C;
		params[i].N = N;
	}

    thread t[NUMTHREADS];

    // cout << "Firing threads ->" << endl;

    for (int i = 0; i < NUMTHREADS; i++)
	{
		t[i] = thread(matrix_multi, &params[i]);
	}

    for (int i = 0; i<NUMTHREADS; i++) {
    	t[i].join();
    } 

    delete [] params;
}

int main () {

    int N = atoi( getenv("MATDIM") );
    double *A = new double[N*N];
    double *B = new double[N*N];
    double *C = new double[N*N];

    // generate random A, B to test
    for (int i=0; i<N; i++) {
        for (int j=0; j<N; j++) {
            A[i*N + j] = i;
            B[i*N + j] = 2*j + i;
        }
    }

    // printMat(A, N, "A");
    // printMat(B, N, "B");
    double st, end;

    get_walltime(&st);
	matrix_mult(A, B, C, N);
	get_walltime(&end);
	cout << "N = " << N << ", P = " << getenv("NUMTHREADS") << ", time taken: " << (end-st);

    // printMat(C, N, "C");

    // transpose(B, N);

    // for (int i = 0; i < N; i++) {
    //     for (int j = 0; j < N; j++) {
    //         C[i*N + j] = 0;
    //     }
    // }
    
    // for (int i = 0; i < N; i++) {
    //     for (int j = 0; j < N; j++) {
    //         for (int k = 0; k < N; k++) {
    //             C[i*N + j] += A[i*N + k] * B[k*N + j] ;
    //         }
    //     }
    // }

    // printMat(C, N, "serial comp. C:");

    free(A); free(B); free(C);

    return 0;
}

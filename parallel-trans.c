#include <iostream>
#include <thread>
#include <stdlib.h>
#include <chrono>
#include <vector>
using namespace std;
using namespace std::chrono;

void trans(int M, int N, int offsetM, int offsetN, int** A, int** B) {
    int l, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    for (int j = offsetN; j < N+offsetN; j+=8) {
        for (int i = offsetM; i < M+offsetM; ++i) {
            tmp0 = A[i][j+0];
            tmp1 = A[i][j+1];
            tmp2 = A[i][j+2];
            tmp3 = A[i][j+3];
            tmp4 = A[i][j+4];
            tmp5 = A[i][j+5];
            tmp6 = A[i][j+6];
            tmp7 = A[i][j+7];
            B[j+0][i] = tmp0;
            B[j+1][i] = tmp1;
            B[j+2][i] = tmp2;
            B[j+3][i] = tmp3;
            B[j+4][i] = tmp4;
            B[j+5][i] = tmp5;
            B[j+6][i] = tmp6;
            B[j+7][i] = tmp7;
        }
    }
}
void parallel_trans_odd_even(int M, int N, int** A, int** B) {
 // one thread handles odd rows and another handles even rows
	thread t1([&](){
		for (int i = 0; i < M; i+=2) {
			for (int j = 0; j < N; ++j)	{
				B[j][i] = A[i][j];
			}
		}
	});
	thread t2([&](){
		for (int i = 1; i < M; i+=2) {
			for (int j = 0; j < N; ++j)	{
				B[j][i] = A[i][j];
			}
		}
	});
	t1.join();
	t2.join();
}
void parallel_trans_2(int M, int N, int** A, int** B) { 
	// your best optimization for 2 threads
	// if (M!=N) {
		// parallel_trans_odd_even(M,N,A,B); return;
	// }
	thread t1([&](){
		trans(M/2, N/2, 0, 0, A, B);
		trans(M/2, N/2, 0, N/2, A, B);
	});
	thread t2([&](){
		trans(M/2, N/2, M/2, 0, A, B);
		trans(M/2, N/2, M/2, N/2, A, B);
	});
	t1.join();
	t2.join();
}
void parallel_trans_4(int M, int N, int** A, int** B) {
	// your best optimization for 4 threads 
	// if (M!=N) {
		// parallel_trans_odd_even(M,N,A,B); return;
	// }
	thread t1([&](){
		trans(M/2, N/2, 0, 0, A, B);
	});
	thread t2([&](){
		trans(M/2, N/2, 0, N/2, A, B);
	});
	thread t3([&](){
		trans(M/2, N/2, M/2, 0, A, B);
	});
	thread t4([&](){
		trans(M/2, N/2, M/2, N/2, A, B);
	});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
}
microseconds test (int m,int n, int type);
int main() {
	srand (time(NULL));
	int ntimes = 500;
	for (int j = 0; j < 5; ++j)	{
		long int avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(16,16,j).count();
		cout << avg/ntimes << endl;

		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(32,32,j).count();
		cout << avg/ntimes << endl;

		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(64,64,j).count();
		cout << avg/ntimes << endl;
		
		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(128,128,j).count();
		cout << avg/ntimes << endl;
		
		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(256,256,j).count();
		cout << avg/ntimes << endl;

		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(512,512,j).count();
		cout << avg/ntimes << endl;

		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(1024,1024,j).count();
		cout << avg/ntimes << endl;

		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(2048,2048,j).count();
		cout << avg/ntimes << endl;

		avg = 0;
		for (int i = 0; i < ntimes; ++i)
			avg += test(4096,4096,j).count();
		cout << avg/ntimes << endl;
	}
	// test(m,n,0);
	// test(m,n,1);
	// test(m,n,2);
	// test(m,n,3);
	// test(m,n,4);
	return 0;
}
microseconds test (int m,int n, int type) {
		int **a = new int *[m];
		int **b = new int *[n];
		// vector< vector< int > > a (m, vector<int> (n, rand()));
		// vector< vector< int > > b (n, vector<int> (m, 0));
		for(int i = 0; i <m; i++) {
		    a[i] = new int[n];
		    for (int j = 0; j < n; ++j) {
			 	a[i][j] = rand();
		    }
		}
		for(int i = 0; i <n; i++) {
		    b[i] = new int[m];
		}

		microseconds start, end;

		start = duration_cast< microseconds >(system_clock::now().time_since_epoch());
		if (type == 0) {
			for (int i = 0; i < m; ++i) {
				for (int j = 0; j < n; ++j)	{
					b[j][i] = a[i][j];
				}
			}
		}
		if (type == 1)
			trans(m,n,0,0,(int**)a,(int**)b);
		if (type == 2)
			parallel_trans_odd_even(m,n,(int**)a,(int**)b);
		if (type == 3)
			parallel_trans_2(m,n,(int**)a,(int**)b);
		if (type == 4)
			parallel_trans_4(m,n,(int**)a,(int**)b);
		end = duration_cast< microseconds >(system_clock::now().time_since_epoch());


		// if (type == 0)
			// cout << "normal time = " << end.count() - start.count() << endl;
		// if (type == 1)
			// cout << "optimized time = " << end.count() - start.count() << endl;
		// if (type == 2)
			// cout << "parallel_trans_odd_even time = " << end.count() - start.count() << endl;
		// if (type == 3)
			// cout << "parallel_trans_2 time = " << end.count() - start.count() << endl;
		// if (type == 4)
			// cout << "parallel_trans_4 time = " << end.count() - start.count() << endl;


		// for (int i = 0; i < m; ++i) {
		// 	for (int j = 0; j < n; ++j) {
		// 		if (a[i][j] != b[j][i]) {
		// 			cout << "FAIL"<<endl; 
		// 			exit(0);
		// 		}
		// 	}
		// }
		for(int i = 0; i <m; i++)
		    delete [] a[i];
		delete[] a;
		for(int i = 0; i <n; i++)
		    delete [] b[i];
		delete[] b;
		return end - start;
}


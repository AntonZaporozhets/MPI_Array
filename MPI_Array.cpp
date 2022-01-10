#include "mpi.h"
#include <iostream>
using namespace std;
double* arr, sum = 0, buffer, totalsum;
int n;
MPI_Status status;


int main() {
	int rank, commSize;
	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &commSize);

	//The main process
	if (rank == 0) {
		cout << "Input a size of array: ";
		cin >> n;

		//Array initialization
		arr = new double[n];
		for (int i = 0; i < n; i++) {
			arr[i] = i;
		}

		int partSize = n / commSize;
		int shift = n % commSize;

		//Send parameters to other threads
		for (int i = 1; i < commSize; i++) {
			MPI_Send(arr + shift + partSize * i, partSize, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		}

		//Calculate the sum of the elements of the first part of the array
		for (int i = 0; i < shift + partSize; i++) {
			sum += arr[i];
		}

		//Calculation of the total sum
		totalsum = sum;
		for (int i = 1; i < commSize; i++) {
			MPI_Recv(&buffer, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);
			totalsum += buffer;
		}
		cout << "total sum = " << totalsum << endl;
	}

	//Parallel processes
	else {
		MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_DOUBLE, &n);

		//Calculation of the partial sum
		arr = new double[n];
		MPI_Recv(arr, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
		for (int i = 0; i < n; i++) {
			sum += arr[i];
		}

		//Send of the calculated partial sum to the main thread
		MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}

	cout << "thread " << rank << " = " << sum << endl;
	MPI_Finalize();
}
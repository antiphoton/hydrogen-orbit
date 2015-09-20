#include"mympi.h"
#include<stdlib.h>
MpiEx::MpiEx() {
	int argc=0;
	char ** argv=0;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	if (!(size>1)) {
		puts("Please run this in mpi.");
		abort();
	}
}
MpiEx::~MpiEx() {
	MPI_Finalize();
}
MpiEx mpi;


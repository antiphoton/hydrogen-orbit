#include<math.h>
#include<stdio.h>
#include<mpi.h>
#include<unistd.h>
#include<algorithm>
#include<fstream>
#include<iostream>
#include<vector>
#include"mymath.h"
#include"mympi.h"
#include"render.h"
#include"monte_carlo.h"
#include"wave.h"
using std::cout;
using std::endl;
using std::make_pair;
using std::ostream;
using std::pair;
using std::vector;
double factor=1;
int main(int argc, char **argv) {
	using namespace std;
	MpiSharedArray<char> a(mpiGlobal.size);
	int i;
	for (i=0;i<26;i++) {
		a[mpiGlobal.rank]='A'+i;
		usleep(rand()*1.0/RAND_MAX*1000000);
		SINGLERUN(
			int j;
			for (j=1;j<mpiGlobal.size;j++) {
				printf("%c",a[j]);
			}
			puts("");
		);
	}
	return 0;
};


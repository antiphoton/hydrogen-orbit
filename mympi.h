#include<mpi.h>
class MpiEx {
	public:
		MpiEx();
		~MpiEx();
		int size,rank;
};
extern MpiEx mpi;

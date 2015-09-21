#include<mpi.h>
#include<stdlib.h>
#include<cstdlib>
#include<vector>
class MpiGlobal {
	public:
		MpiGlobal();
		~MpiGlobal();
		int size,rank;
};
extern MpiGlobal mpiGlobal;
class MpiTaskManager {
	public:
		MpiTaskManager();
		~MpiTaskManager();
		bool isMyTask(int hash);
		void submitSlowTask(int hash);
		void submitQuickTask(int hash);
	private:
		void analyze(int iThread);
		int whoShouldDo(int hash);
		bool head;
		long totalSlow,totalQuick;
		long finishSlow;
		timespec *threadTime;
		long *threadJob,*threadDone;
		double cost,cost2;
};

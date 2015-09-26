#include<mpi.h>
#include<pthread.h>
#include<stdlib.h>
#include<cstdlib>
#include<string>
#include<vector>
enum MPI_TAG {
	MPI_TASK_SLOW,
	MPI_TASK_QUICK,
	MPI_HIST_MERGE,
	MPI_MEMORY_READY,
	MPI_SYNC,
	MPI_SINGLETHREAD,
};
class MpiGlobal {
	public:
		MpiGlobal();
		~MpiGlobal();
		bool head() const;
		int size,rank;
	private:
		static int count;
};
extern MpiGlobal mpiGlobal;
class MpiChecker {
	public:
		MpiChecker();
};
void mpiSync();
class MpiSharedMemory {
	public:
		MpiSharedMemory(int size);
		~MpiSharedMemory();
		void * address() const;
	protected:
		void *p;
	private:
		MpiChecker mpiChecker;
};
template<typename T> class MpiSharedArray:MpiSharedMemory { 
	public:
		MpiSharedArray(int size):MpiSharedMemory(sizeof(T)*size) {
		}
		T & operator [] (int i) {
			return *((T*)p+i);
		}
};
class SingleThreadLocker {
	public:
		SingleThreadLocker(bool includingHead=false);
		~SingleThreadLocker();
		bool myDuty();
	private:
		static pthread_mutex_t *pMutex;
		static bool staticInited;
		const bool includingHead;
		static const int WORKING_RANK;
};
#define SINGLERUN(f) {SingleThreadLocker __cbxLocker__; if (__cbxLocker__.myDuty()) {f}}
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
		MpiChecker mpiChecker;
};
class ParallelHistogram {
	public:
		ParallelHistogram(const bool logScale,const bool clamped,const double min,const double max,const int count,const std::string &filename);
		~ParallelHistogram();
		void tip(double x);
		double mu() const;
		double sigma() const;
	private:
		void merge();
		void write() const;
		const bool logScale;
		const bool clamped;
		const double min,max;
		const int count;
		const std::string filename;
		long *a;
		double _mu,_sigma;
		MpiChecker mpiChecker;
};


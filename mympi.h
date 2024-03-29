#include<mpi.h>
#include<pthread.h>
#include<stdlib.h>
#include<cstdlib>
#include<string>
#include<vector>
enum MPI_TAG {
	MPI_TASK_APPLY,
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
		bool myDuty(int i) const;
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
		inline T & operator [] (int i) {
			return *address(i);
		}
		inline T * address(int i=0) {
			return (T*)p+i;
		}
};
template<typename T> class MpiSharedArray2:MpiSharedMemory {
	public:
		MpiSharedArray2(int s1,int s2):MpiSharedMemory(sizeof(T)*s1*s2),s1(s1),s2(s2) {
		}
		inline T * address(int i1,int i2) {
			return (T*)p+i1*s2+i2;
		}
	private:
		const int s1,s2;
};
template<typename T> class MpiSharedArray3:MpiSharedMemory {
	public:
		MpiSharedArray3(int s1,int s2,int s3):MpiSharedMemory(sizeof(T)*s1*s2*s3),s1(s1),s2(s2),s3(s3) {
		}
		inline T * address(int i1,int i2,int i3) {
			return (T*)p+i1*(s2*s3)+i2*s3+i3;
		}
	private:
		const int s1,s2,s3;
};
template<typename T> class MpiSharedArray4:MpiSharedMemory {
	public:
		MpiSharedArray4(int s1,int s2,int s3,int s4):MpiSharedMemory(sizeof(T)*s1*s2*s3*s4),s1(s1),s2(s2),s3(s3),s4(s4) {
		}
		inline T * address(int i1,int i2,int i3,int i4) {
			return (T*)p+i1*(s2*s3*s4)+i2*(s3*s4)+i3*s4+i4;
		}
	private:
		const int s1,s2,s3,s4;
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
		MpiTaskManager(const std::string &tag,int count,double supposedCost);
		~MpiTaskManager();
		int apply();
	private:
		void listen();
		void write();
		const int count;
		const double supposedCost;
		const std::string tag;
		double cost1,cost2;
		MpiChecker mpiChecker;
};
class ParallelHistogram {
	public:
		ParallelHistogram(const bool logScale,const bool clamped,const double min,const double max,const int count,const std::string &filename);
		~ParallelHistogram();
		void tip(double x);
		void writeRecommand(int depth);
	private:
		void merge();
		void write(int depth) const;
		const bool logScale;
		const bool clamped;
		const double min,max;
		const int count;
		const std::string filename;
		long *a;
		double mu,sigma;
		MpiChecker mpiChecker;
};


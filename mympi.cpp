#include<math.h>
#include<string.h>
#include<ctime>
#include<errno.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include"mympi.h"
using std::vector;
int MpiGlobal::count=0;
MpiGlobal::MpiGlobal() {
	if (++count>1) {
		puts("Only one MpiGlobal is allowed.");
		abort();
	}
	int argc=0;
	char ** argv=0;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
}
MpiGlobal::~MpiGlobal() {
	MPI_Finalize();
}
bool MpiGlobal::head() const {
	return rank==0;
}
bool MpiGlobal::myDuty(int i) const {
	return i%(size-1)==rank-1;
}
MpiGlobal mpiGlobal;
MpiChecker::MpiChecker() {
	if (!(mpiGlobal.size>1)) {
		puts("Please run this in mpi.");
		abort();
	}
}
void mpiSync() {
	static MpiChecker mpiChecker;
	if (mpiGlobal.head()) {
		int i;
		for (i=1;i<mpiGlobal.size;i++) {
			MPI_Status stat;
			int x;
			MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_SYNC,MPI_COMM_WORLD,&stat);
		}
	}
	else {
		int x;
		MPI_Send(&x,1,MPI_INT,0,MPI_SYNC,MPI_COMM_WORLD);
	}
};
MpiSharedMemory::MpiSharedMemory(int size) {
	int shmid;
	if (mpiGlobal.head()) {
		shmid=shmget(0,size,IPC_CREAT|0666);
		int i;
		for (i=1;i<mpiGlobal.size;i++) {
			MPI_Send(&shmid,1,MPI_INT,i,MPI_MEMORY_READY,MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Status stat;
		MPI_Recv(&shmid,1,MPI_INT,0,MPI_MEMORY_READY,MPI_COMM_WORLD,&stat);
	}
	p=(unsigned char*) shmat(shmid,NULL,0);
}
MpiSharedMemory::~MpiSharedMemory() {
	mpiSync();
	shmdt(p);
}
void *MpiSharedMemory::address() const {
	return p;
}
double myTimediff(timespec t1,timespec t2) {
	return (t1.tv_sec-t2.tv_sec)+(t1.tv_nsec-t2.tv_nsec)*1e-9;
}
timespec getTime() {
	timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	return now;
}
pthread_mutex_t *SingleThreadLocker::pMutex;
bool SingleThreadLocker::staticInited=false;
const int SingleThreadLocker::WORKING_RANK=1;
SingleThreadLocker::SingleThreadLocker(bool includingHead):includingHead(includingHead) {
	if (!includingHead&&mpiGlobal.head()) {
		return ;
	}
	if (mpiGlobal.rank==WORKING_RANK) {
		int i;
		for (i=0;i<mpiGlobal.size;i++) {
			if ((i==0&&!includingHead)||i==WORKING_RANK) {
				continue;
			}
			MPI_Status stat;
			int x;
			MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_SINGLETHREAD,MPI_COMM_WORLD,&stat);
		}
	}
	else {
		int x;
		MPI_Send(&x,1,MPI_INT,WORKING_RANK,MPI_SINGLETHREAD,MPI_COMM_WORLD);
	}
}
bool SingleThreadLocker::myDuty() {
	return mpiGlobal.rank==WORKING_RANK;
}
SingleThreadLocker::~SingleThreadLocker() {
	if (!includingHead&&mpiGlobal.head()) {
		return ;
	}
	if (mpiGlobal.rank==WORKING_RANK) {
		int i;
		for (i=0;i<mpiGlobal.size;i++) {
			if ((i==0&&!includingHead)||i==WORKING_RANK) {
				continue;
			}
			int x;
			MPI_Send(&x,1,MPI_INT,i,MPI_SINGLETHREAD,MPI_COMM_WORLD);
		}
	}
	else {
		MPI_Status stat;
		int x;
		MPI_Recv(&x,1,MPI_INT,WORKING_RANK,MPI_SINGLETHREAD,MPI_COMM_WORLD,&stat);
	}
}
MpiTaskManager::MpiTaskManager() {
	head=mpiGlobal.rank==0;
	if (!head) {
		return ;
	}
	totalSlow=0;
	totalQuick=0;
	finishSlow=0;
	threadTime=new timespec[mpiGlobal.size];
	threadJob=new long[mpiGlobal.size];
	threadDone=new long[mpiGlobal.size];
	int i;
	for (i=0;i<mpiGlobal.size;i++) {
		threadTime[i]=getTime();
		threadJob[i]=0;
		threadDone[i]=0;
	}
	cost=0;
	cost2=0;
}
MpiTaskManager::~MpiTaskManager() {
	if (!head) {
		return ;
	}
	MPI_Status stat;
	int x;
	int i;
	for (i=0;i<totalSlow;i++) {
		MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_TASK_SLOW,MPI_COMM_WORLD,&stat);
		analyze(x);
	}
	for (i=0;i<totalQuick;i++) {
		MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,MPI_TASK_QUICK,MPI_COMM_WORLD,&stat);
	}
	printf("\n");
	delete[] threadTime;
	delete[] threadJob;
	delete[] threadDone;
}
void MpiTaskManager::analyze(int iThread) {
	finishSlow++;
	threadDone[iThread]++;
	timespec now=getTime();
	double delta=myTimediff(now,threadTime[iThread]);
	threadTime[iThread]=now;
	cost+=delta;
	cost2+=delta*delta;
	double mu=cost/finishSlow;
	//double sigma=sqrt((cost2/finishSlow-mu*mu)/finishSlow);
	double slowest=0,current;
	int i;
	for (i=0;i<mpiGlobal.size;i++) {
		current=(threadJob[i]-threadDone[i])*mu;
		current-=myTimediff(now,threadTime[i]);
		if (current>slowest) {
			slowest=current;
		}
	}
	if (slowest<=99) {
		printf("Estimated Remaining: %2d seconds            \r",(int)(slowest+0.5));
	}
	else {
		time_t f=now.tv_sec+now.tv_nsec/1e9+slowest+0.5;
		tm *timeinfo=localtime(&f);
		static char timeBuffer[256];
		strftime(timeBuffer,256,"%F %X",timeinfo);
		printf("Estiamted Time:     %s\r",timeBuffer);
	}
}
int MpiTaskManager::whoShouldDo(int hash) {
	return hash%(mpiGlobal.size-1)+1;
}
bool MpiTaskManager::isMyTask(int hash) {
	return whoShouldDo(hash)==mpiGlobal.rank;
}
void MpiTaskManager::submitSlowTask(int hash) {
	if (head) {
		totalSlow++;
		threadJob[whoShouldDo(hash)]++;
	}
	else {
		if (isMyTask(hash)) {
			MPI_Send(&mpiGlobal.rank,1,MPI_INT,0,MPI_TASK_SLOW,MPI_COMM_WORLD);
		}
	}
}
void MpiTaskManager::submitQuickTask(int hash) {
	if (head) {
		totalQuick++;
	}
	else {
		if (isMyTask(hash)) {
			MPI_Send(&mpiGlobal.rank,1,MPI_INT,0,MPI_TASK_QUICK,MPI_COMM_WORLD);
		}
	}
}
ParallelHistogram::ParallelHistogram(const bool logScale,const bool clamped,const double min,const double max,const int count,const std::string &filename):logScale(logScale),clamped(clamped),min(logScale?log(min):min),max(logScale?log(max):max),count(count),filename(filename) {
	a=new long[count];
	memset(a,0,sizeof(long)*count);
};
ParallelHistogram::~ParallelHistogram() {
	delete[] a;
}
void ParallelHistogram::tip(double x) {
	if (logScale) {
		x=log(x);
	}
	int i=(x-min)/(max-min)*count;
	if (i<0) {
		if (!clamped) {
			return ;
		}
		else {
			i=0;
		}
	}
	if (i>=count) {
		if (!clamped) {
			return ;
		}
		else {
			i=count-1;
		}
	}
	a[i]++;
}
void ParallelHistogram::writeRecommand(int depth) {
	merge();
	write(depth);
}
void ParallelHistogram::merge() {
	if (!mpiGlobal.head()) {
		MPI_Send(a,count*8,MPI_CHAR,0,MPI_HIST_MERGE,MPI_COMM_WORLD);
	}
	else {
		MPI_Status mpiStat;
		int i,j;
		long *buffer=new long[count];
		for (i=1;i<mpiGlobal.size;i++) {
			MPI_Recv(buffer,count*8,MPI_CHAR,MPI_ANY_SOURCE,MPI_HIST_MERGE,MPI_COMM_WORLD,&mpiStat);
			for (j=0;j<count;j++) {
				a[j]+=buffer[j];
			}
		}
		delete[] buffer;
		long total=0;
		double sum1=0,sum2=0;
		for (i=0;i<count;i++) {
			double x=(i+0.5)/count*(max-min)+min;
			if (logScale) {
				x=exp(x);
			}
			total+=a[i];
			sum1+=x*a[i];
			sum2+=x*x*a[i];
		}
		mu=sum1/total;
		sigma=sqrt(sum2/total-mu*mu);
	}
};
void ParallelHistogram::write(int depth) const {
	if (!mpiGlobal.head()) {
		return ;
	}
	puts("Recommended factor");
	int i;
	for (i=0;i<3;i++) {
		printf("%8.3f%c",sqrt((i+1)/(mu+sigma/sqrt(depth))),i<3-1?'\t':'\n');
	}
	
}

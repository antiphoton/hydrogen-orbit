#include<math.h>
#include<ctime>
#include"mympi.h"
using std::vector;
MpiGlobal::MpiGlobal() {
	int argc=0;
	char ** argv=0;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
}
MpiGlobal::~MpiGlobal() {
	MPI_Finalize();
}
MpiGlobal mpiGlobal;
const int TAG_SLOW=1,TAG_QUICK=2;
double myTimediff(timespec t1,timespec t2) {
	return (t1.tv_sec-t2.tv_sec)+(t1.tv_nsec-t2.tv_nsec)*1e-9;
}
timespec getTime() {
	timespec now;
	clock_gettime(CLOCK_REALTIME,&now);
	return now;
}
MpiTaskManager::MpiTaskManager() {
	bool first=false;
	if (first&&!(mpiGlobal.size>1)) {
		puts("Please run this in mpi.");
		abort();
		first=true;
	}
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
		MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,TAG_SLOW,MPI_COMM_WORLD,&stat);
		analyze(x);
	}
	for (i=0;i<totalQuick;i++) {
		MPI_Recv(&x,1,MPI_INT,MPI_ANY_SOURCE,TAG_QUICK,MPI_COMM_WORLD,&stat);
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
			MPI_Send(&mpiGlobal.rank,1,MPI_INT,0,TAG_SLOW,MPI_COMM_WORLD);
		}
	}
}
void MpiTaskManager::submitQuickTask(int hash) {
	if (head) {
		totalQuick++;
	}
	else {
		if (isMyTask(hash)) {
			MPI_Send(&mpiGlobal.rank,1,MPI_INT,0,TAG_QUICK,MPI_COMM_WORLD);
		}
	}
}


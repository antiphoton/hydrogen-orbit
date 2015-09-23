#include<math.h>
#include<stdio.h>
#include<mpi.h>
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
struct Eigenstate {
	int n,l,m;
	RadialWave *fR;
	SphericalHarmonic *fS;
	Complex weight;
	double energy;
	Eigenstate(int n,int l,int m):n(n),l(l),m(m),weight(0,0) {
		fR=new RadialWave(n,l);
		fS=new SphericalHarmonic(l,m);
	}
};
ostream & operator << (ostream &cout,const Eigenstate &s) {
	return cout<<"n="<<s.n<<"\tl="<<s.l<<"\tm="<<s.m<<"\t\t"<<s.weight;
}
vector<Eigenstate> eigen;
Vector3 rMu,rSigma,waveNumber;
RadialWave *currentRadial;
SphericalHarmonic *currentSpherical;
double currentEnergy;
Complex f0(double rho,double theta,double phi) {
	return currentSpherical->calc(theta,phi)*currentRadial->calc(rho);
}
Complex f1(double rho,double theta,double phi,double t) {
	const double frequency=1.0/5;
	const double omega=2*acos(-1.0)*frequency;
	Complex ret(0,0);
	int i;
	for (i=0;i<(int)eigen.size();i++) {
		Complex cur=eigen[i].fS->calc(theta,phi)*eigen[i].fR->calc(rho);
		double p=omega*t*eigen[i].energy;
		cur=cur*Complex(cos(p),sin(p));
		ret=ret+cur;
	}
	return ret*factor;
}
void createWaveFunction() {
	int n,l,m;
	for (n=1;n<=6;n++) {
		for (l=0;l<n;l++) {
			for (m=-l;m<=l;m++) {
				eigen.push_back(Eigenstate(n,l,m));
			}
		}
	}
}
void decompose() {
	int i;
	for (i=0;i<(int)eigen.size();i++) {
		currentRadial=eigen[i].fR;
		currentSpherical=eigen[i].fS;
		Complex z=integrateNormal3(f0,rMu,rSigma,waveNumber);
		eigen[i].weight=z;
	}

}
void writeWeight() {
	int n=eigen.size();
	int *a=new int[n];
	int i,j,k;
	for (i=0;i<n;i++) {
		a[i]=i;
	}
	for (i=0;i<n;i++) {
		for (j=i+1;j<n;j++) {
			if (eigen[a[i]].weight.length()<eigen[a[j]].weight.length()) {
				k=a[i];
				a[i]=a[j];
				a[j]=k;
			}
		}
	}
	std::ofstream fout("output/weight.txt");
	for (i=0;i<n;i++) {
		fout<<eigen[a[i]]<<endl;
	}
	delete[] a;
}

int main(int argc, char **argv) {
	factor=5;
	rMu=Vector3(4,0,0);
	rSigma=Vector3(0.1,0.1,0.1);
	waveNumber=Vector3(0,-10,0);
	createWaveFunction();
	decompose();
	//k[0]=Complex(1,0);
	writeWeight();
	const int w=200,h=200,l=720;
	SphericalFunctionPlotter sp(f1,w,h,0.02,l,"/home/cbx/Dropbox/nodejs/web/buffer/out.mp4","jpeg");
	return 0;
};


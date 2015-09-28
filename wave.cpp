#include<math.h>
#include<stdio.h>
#include<iostream>
#include<fstream>
#include"mymath.h"
#include"mympi.h"
#include"monte_carlo.h"
#include"render.h"
#include"wave.h"
using std::cout;
using std::endl;
const static double PI=acos(-1.0);
Legendre::Legendre(int n):n(n) {
	Polynomial b(2*n+1);
	b[2*n]=1;
	int i;
	for (i=n-1;i>=0;i--) {
		b[2*i]=-b[2*(i+1)]*(i+1)/(n-i);
	}
	double k=1;
	for (i=1;i<=n;i++) {
		k/=2*i;
	}
	a=(b>>n)*k;
}
Legendre::~Legendre() {
}
AssociatedLegendre::AssociatedLegendre(int l,int m):l(l),m(m) {
	Legendre p0(l);
	a=p0.a>>m;
}
AssociatedLegendre::~AssociatedLegendre() {
}
double AssociatedLegendre::calc(double x) const {
	double xx=1;
	double y=0;
	int i;
	for (i=0;i<=l-m;i++) {
		y+=a[i]*xx;
		xx*=x;
	}
	if (m%2==1) {
		y=-y;
	}
	return pow(1-x*x,m/2.0)*y;
}
RadialWave::RadialWave(int n,int l):n(n),l(l),a(n-l) {
	int k;
	a[0]=1;
	for (k=0;k<n-l-1;k++) {
		a[k+1]=a[k]*2/n*(k+l+1-n)/((k+1)*(k+2*l+2));
	}
	Polynomial p(3+2*l);
	p[2+2*l]=1;
	p=p*a*a;
	double c=p.integrateExponential(2.0/n);
	c=1/sqrt(c);
	a=a*c;
}
double RadialWave::calc(double x) const {
	double y=0;
	int k;
	for (k=0;k<n-l;k++) {
		y+=a[k]*pow(x,l+k)*exp(-x/n);
	}
	return y;
}
double RadialWave::integrated(double x) const {
	double y=calc(x);
	return x*x*y*y;
}
SphericalHarmonic::SphericalHarmonic(int l,int m):l(l),m(m) {
	pal=new AssociatedLegendre(l,m>=0?m:-m);
	a=1;
	double c=simpson(*this,0,PI);
	a=1/sqrt(c);
	if (m<0&&(-m)%2==1) {
		a=-a;
	}
}
SphericalHarmonic::~SphericalHarmonic() {
	delete pal;
}
Complex SphericalHarmonic::calc(double theta,double phi) const {
	return a*pal->calc(cos(theta))*Complex(cos(m*phi),sin(m*phi));
}
double SphericalHarmonic::integrated(double theta) const {
	double y=a*pal->calc(cos(theta));
	return (2*PI)*sin(theta)*y*y;
}
BasisSet::Eigenstate::Eigenstate(int n,int l,int m):n(n),l(l),m(m) {
	fR=new RadialWave(n,l);
	fS=new SphericalHarmonic(l,m);
	energy=-1.0/n/n;
}
BasisSet::BasisSet(int maxN) {
	int n,l,m;
	for (n=1;n<=maxN;n++) {
		for (l=0;l<n;l++) {
			for (m=-l;m<=l;m++) {
				v.push_back(Eigenstate(n,l,m));
			}
		}
	}
}
BasisSet::~BasisSet() {
}
namespace toBeComposed {
	RadialWave *currentRadial;
	SphericalHarmonic *currentSpherical;
	Complex f0(double rho,double theta,double phi) {
		return currentSpherical->calc(theta,phi)*currentRadial->calc(rho);
	}
};
void BasisSet::project(const WavePacket &wave) {
	int n=v.size();
	int i;
	for (i=0;i<n;i++) {
		toBeComposed::currentRadial=v[i].fR;
		toBeComposed::currentSpherical=v[i].fS;
		Complex z=integrateSqrtNormal3(toBeComposed::f0,wave);
		v[i].weight=z;
	}
}
int BasisSet::getSize() const {
	return v.size();
}
void BasisSet::getEnergy(double *a) const {
	int n=v.size();
	int i;
	for (i=0;i<n;i++) {
		a[i]=v[i].energy;
	}
}
void BasisSet::getValueByCartesian(Complex *a,const Vector3 &p) const {
	const double &x=p.x;
	const double &y=p.y;
	const double &z=p.z;
	int n=v.size();
	double rho,theta,phi;
	rho=sqrt(x*x+y*y+z*z);
	if (rho==0) {
		theta=0;
		phi=0;
	}
	else {
		theta=acos(z/rho);
		phi=atan2(y,x);
	}
	int i;
	for (i=0;i<n;i++) {
		a[i]=v[i].fR->calc(rho)*v[i].fS->calc(theta,phi)*v[i].weight;
	}
}
std::ostream & operator << (std::ostream &cout,const BasisSet::Eigenstate &s) {
	return cout<<"n="<<s.n<<"\tr="<<(s.n-1-s.l)<<"\tl="<<s.l<<"\tm="<<s.m;
}
void BasisSet::writeWeight() const {
	int n=v.size();
	int *a=new int[n];
	int i,j,k;
	for (i=0;i<n;i++) {
		a[i]=i;
	}
	for (i=0;i<n;i++) {
		for (j=i+1;j<n;j++) {
			if (v[a[i]].weight.lengthSqr()<v[a[j]].weight.lengthSqr()) {
				k=a[i];
				a[i]=a[j];
				a[j]=k;
			}
		}
	}
	std::ofstream fout("output/weight.txt");
	for (i=0;i<n;i++) {
		fout<<v[a[i]]<<"\t"<<v[a[i]].weight<<endl;
	}
	delete[] a;
}


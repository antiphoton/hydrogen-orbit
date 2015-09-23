#include<math.h>
#include<stdio.h>
#include<iostream>
#include"mymath.h"
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
SphericalHarmonic::SphericalHarmonic(int l,int m):l(l),m(m),pal(l,m) {
	a=1;
	double c=simpson(*this,0,PI);
	a=1/sqrt(c);
}
SphericalHarmonic::~SphericalHarmonic() {
}
Complex SphericalHarmonic::calc(double theta,double phi) const {
	double y=a*pal.calc(cos(theta));
	return Complex(y*cos(phi),y*sin(phi));
}
double SphericalHarmonic::integrated(double theta) const {
	double y=a*pal.calc(cos(theta));
	return (2*PI)*sin(theta)*y*y;
}
void init_wave() {
}
static Complex f1(double r,double theta,double phi,double t) {
	const int n=6,l=3,m=2;
	static RadialWave a(n,l);
	static SphericalHarmonic b(l,m);
	return Complex(a.calc(r),0)*b.calc(theta,phi)*150;
}
void test_wave() {
	const int w=100,h=100,l=8;
	SphericalFunctionPlotter sp(f1,w,h,1.0/5/5/5,l,"/home/cbx/buffer/Monday/out.mp4","jpeg");//Dropbox/nodejs/web/buffer/out.mp4","jpeg");
	//const int n=3,l=7,m=3;
	//f1(0,0,0,0);
	//while (1);
	//static RadialWave a(n,l);
};


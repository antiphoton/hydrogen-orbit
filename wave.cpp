#include<math.h>
#include<stdio.h>
#include<iostream>
#include"geom.h"
#include"draw.h"
#include"monte_carlo.h"
#include"render.h"
#include"wave.h"
const static double PI=acos(-1.0);
void diffPoly(double *a,int n,int d=1) {
	int i,j;
	for (i=0;i<n;i++) {
		if (i+d<n) {
			double f=1;
			for (j=i+d;j>i;j--) {
				f*=j;
			}
			a[i]=f*a[i+d];
		}
		else {
			a[i]=0;
		}
	}
}
Legendre::Legendre(int n):n(n) {
	double *b=new double[2*n+1];
	int i;
	for (i=0;i<2*n+1;i++) {
		b[i]=0;
	}
	b[2*n]=1;
	for (i=n-1;i>=0;i--) {
		b[2*i]=-b[2*(i+1)]*(i+1)/(n-i);
	}
	diffPoly(b,2*n+1,n);
	a=new double[n+1];
	double k=1;
	for (i=1;i<=n;i++) {
		k/=2*i;
	}
	for (i=0;i<=n;i++) {
		a[i]=b[i]*k;
	}
	delete[] b;
}
Legendre::~Legendre() {
	delete[] a;
}
AssociatedLegendre::AssociatedLegendre(int l,int m):l(l),m(m) {
	Legendre p0(l);
	diffPoly(p0.a,l+1,m);
	a=new double[l-m+1];
	int i;
	for (i=0;i<=l-m;i++) {
		a[i]=p0.a[i];
	}
	//double c;
}
AssociatedLegendre::~AssociatedLegendre() {
	delete[] a;
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
RadialWave::RadialWave(int n,int l):n(n),l(l) {
	int k;
	a[0]=1;
	for (k=0;k<n-l-1;k++) {
		a[k+1]=a[k]*2/n*(k+l+1-n)/((k+1)*(k+2*l+2));
	}
	double c=simpsonHalfInf(*this);
	c=1/sqrt(c);
	for (k=0;k<n-l;k++) {
		a[k]*=c;
	}
}
double RadialWave::integrated(double x) const {
	double y=0;
	int k;
	for (k=0;k<n-l;k++) {
		y+=a[k]*pow(x,l+k)*exp(-x/n);
	}
	//printf("f(%f)=%f\n",x,y);
	return x*x*y*y;
}
SphericalHarmonic::SphericalHarmonic(int l,int m):l(l),m(m),pal(l,m) {
	a=1;
	double c=simpson(*this,0,PI);
	printf("%f\n",c);
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
//RadialWave rw(3,1);
static Complex f1(double r,double theta,double phi,double t) {
	static SphericalHarmonic b(1,1);
	return b.calc(theta,phi);
}
void test_wave() {
	const int w=200,h=200,l=8;
	SphericalFunctionPlotter sp(f1,w,h,0.5,l,"/home/cbx/buffer/Monday/out.mp4","jpeg");//Dropbox/nodejs/web/buffer/out.mp4","jpeg");
};


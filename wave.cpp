#include<math.h>
#include<stdio.h>
#include<iostream>
#include"geom.h"
#include"monte_carlo.h"
#include"wave.h"
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
		printf("a[%d]=%f\n",k,a[k]);
	}
}
double RadialWave::calc(double x) const {
	double y=0;
	int k;
	for (k=0;k<n-l;k++) {
		y+=a[k]*pow(x,l+k)*exp(-x/n);
	}
	//printf("f(%f)=%f\n",x,y);
	return x*x*y*y;
}
void init_wave() {
}
double radialWave(int n,int l,double rho) {
}
void test_wave() {
	init_wave();
	RadialWave r(3,1);
};


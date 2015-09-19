#include<stdio.h>
#include<math.h>
#include<cstdlib>
#include"monte_carlo.h"
inline double getRandom() {
	return 1.0*rand()/RAND_MAX;
}
double getRandomGauss(double mu=0,double sigma=1) {
	double x=0;
	int i;
	for (i=0;i<12;i++) {
		x+=getRandom();
	}
	return (x-6)*sigma+mu;
}
double getRandomExponential(double k=1) {
	return 1/k*log(1/(1-getRandom()));
}

inline double parabolaIntegrate(const Function11 &f,double a,double b) {
	return (f.integrated(a)+f.integrated((a+b)/2)*4+f.integrated(b))*(b-a)/6;
}
double simpson(const Function11 &f,double a,double b,double eps,double sT) {
	double c=(a+b)/2;
	double sL=parabolaIntegrate(f,a,c);
	double sR=parabolaIntegrate(f,c,b);
	if (fabs(sL+sR-sT)<=15*eps) {
		return sL+sR+(sL+sR-sT)/15;
	}
	else {
		return simpson(f,a,c,eps/2,sL)+simpson(f,c,b,eps/2,sR);
	}
}
double simpson(const Function11 &f,double a,double b,double eps) {
	return simpson(f,a,b,eps,parabolaIntegrate(f,a,b));
}
double simpsonHalfInf(const Function11 &f,double eps) {
	double m=0.987654321;
	double s=simpson(f,0,m,eps);
	while (1) {
		double y=simpson(f,m,m*2,eps);
		s+=y;
		if (y<eps) {
			break;
		}
		m*=2;
	}
	return s;
}



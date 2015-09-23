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
Complex integrateNormal3(Complex (*f)(double r,double theta,double phi),const Vector3 &pMu,const Vector3 &pSigma,const Vector3 &waveNumber,double eps) {
	srand(time(0));
	double ansX1=0,ansY1=0;
	double ansX2=0,ansY2=0;
	const int MIN_TEST=50;
	long n=0;
	while (1) {
		double x=getRandomGauss(0,pSigma.x);
		double y=getRandomGauss(0,pSigma.y);
		double z=getRandomGauss(0,pSigma.z);
		double kr=x*waveNumber.x+y*waveNumber.y+z*waveNumber.z;
		x+=pMu.x;
		y+=pMu.y;
		z+=pMu.z;
		double r=sqrt(x*x+y*y+z*z);
		double theta,phi;
		if (r==0) {
			theta=0;
			phi=0;
		}
		else {
			theta=acos(z/r);
			phi=atan2(y,x);
		}
		Complex c=(!f(r,theta,phi))*Complex(cos(kr),sin(kr));
		ansX1+=c.x;
		ansY1+=c.y;
		ansX2+=c.x*c.x;
		ansY2+=c.y*c.y;
		n++;
		if (n<MIN_TEST) {
			continue;
		}
		double xMu=ansX1/n;
		double yMu=ansY1/n;
		double xSigma=sqrt((ansX2/n-xMu*xMu)/n);
		double ySigma=sqrt((ansY2/n-yMu*yMu)/n);
		if (xSigma<=eps&&ySigma<=eps) {
			break;
		}
	}
	return Complex(ansX1/n,ansY1/n);
}


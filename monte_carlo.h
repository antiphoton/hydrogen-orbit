#pragma once
#include"mymath.h"
struct Function11 {
	virtual double integrated(double) const=0;
};
struct Function21 {
	virtual double integrated(double,double) const=0;
};
double simpson(const Function11 &f,double a,double b,double eps=1e-9);
double simpsonHalfInf(const Function11 &f,double eps=1e-9);
Complex integrateSqrtNormal3(Complex (*f)(double r,double theta,double phi),const Vector3 &pMu,Vector3 pSigma,const Vector3 &waveNumber,double eps=5e-5);


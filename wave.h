#pragma once
const int MAX_N=10;
struct Legendre {
	double *a;
	int n;
	Legendre(int n);
	~Legendre();
};
struct AssociatedLegendre {
	double *a;
	int l,m;
	AssociatedLegendre(int l,int m);
	~AssociatedLegendre();
	double calc(double x) const;
};
struct RadialWave:Function11 {
	double a[MAX_N];
	int n,l;
	RadialWave(int n,int l);
	double integrated(double x) const ;
};
struct SphericalHarmonic:Function11 {
	double a;
	int l,m;
	AssociatedLegendre pal;
	SphericalHarmonic(int l,int m);
	~SphericalHarmonic();
	Complex calc(double theta,double phi) const ;
	double integrated(double x) const;
};
struct Hydrogen {
	int n,l,m;
	Hydrogen(int n,int l,int m);
};
void init_wave();
void test_wave();

#pragma once
struct Legendre {
	int n;
	Polynomial a;
	Legendre(int n);
	~Legendre();
};
struct AssociatedLegendre {
	int l,m;
	Polynomial a;
	AssociatedLegendre(int l,int m);
	~AssociatedLegendre();
	double calc(double x) const;
};
struct RadialWave {
	int n,l;
	Polynomial a;
	RadialWave(int n,int l);
	double calc(double rho) const;
	double integrated(double x) const ;
};
struct SphericalHarmonic:Function11 {
	double a;
	int l,m;
	AssociatedLegendre *pal;
	SphericalHarmonic(int l,int m);
	~SphericalHarmonic();
	Complex calc(double theta,double phi) const ;
	double integrated(double x) const;
};
struct Hydrogen {
	int n,l,m;
	Hydrogen(int n,int l,int m);
};


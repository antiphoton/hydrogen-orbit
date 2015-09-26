#pragma once
#include"mymath.h"
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
struct SphericalHarmonic:Integrated11 {
	double a;
	int l,m;
	AssociatedLegendre *pal;
	SphericalHarmonic(int l,int m);
	~SphericalHarmonic();
	double calc(double theta) const ;
	double integrated(double x) const;
};
class BasisSet {
	public:
		struct Eigenstate {
			int n,l,m;
			Eigenstate(int n,int l,int m);
			RadialWave *fR;
			SphericalHarmonic *fS;
			Complex weight;
			double energy;
		};
		BasisSet(int maxN);
		void project(const WavePacket &wave);
		int getSize() const;
		void getEnergy(double *a) const;
		void getValueByCartesian(Complex *a,const double x,const double y,const double z) const;
		void writeWeight() const;
		~BasisSet();
	private:
		std::vector<Eigenstate> v;
};


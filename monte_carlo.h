struct Function11 {
	virtual double integrated(double) const=0;
};
struct Function21 {
	virtual double integrated(double,double) const=0;
};
double simpson(const Function11 &f,double a,double b,double eps=1e-9);
double simpsonHalfInf(const Function11 &f,double eps=1e-9);

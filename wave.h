const int MAX_N=3;
struct RadialWave:Function11 {
	double a[MAX_N];
	int n,l;
	RadialWave(int n,int l);
	double calc(double x) const ;
};
void init_wave();
void test_wave();

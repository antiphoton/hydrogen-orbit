#include<math.h>
#include<stdio.h>
#include<iostream>
#include"color.h"
#include"mympi.h"
const static double PI=acos(-1.0);
inline static double clip(double x) {
	if (x<0) {
		return 0;
	}
	if (x>1) {
		return 1;
	}
	return x;
}
static double hue2rgb(double p,double q,double t) {
	t-=floor(t);
	if (t<1.0/6) {
		return p+(q-p)*6*t;
	}
	if (t<1.0/2) {
		return q;
	}
	if (t<2.0/3) {
		return p+(q-p)*6*(2.0/3-t);
	}
	return p;
}
ColorHsl::ColorHsl(double h,double s,double l):h(h),s(s),l(l) {
}
std::ostream & operator << (std::ostream & cout,const ColorHsl &c) {
	static char s[255];
	sprintf(s,"(H=%3.2f,S=%3.2f,L=%3.2f)",c.h,c.s,c.l);
	return cout<<s;
}
std::ostream & operator << (std::ostream & cout,const ColorRgbA &c) {
	static char s[255];
	sprintf(s,"(R=%3.2f,G=%3.2f,B=%3.2f)",c.r,c.g,c.b);
	return cout<<s;
}
ColorYuv::ColorYuv(double y,double u,double v):y(y),u(u),v(v) {
}
ColorRgbA::ColorRgbA(double r,double g,double b,double a):r(r),g(g),b(b),a(a) {
}
ColorRgbA operator + (const ColorRgbA &c1,const ColorRgbA &c2) {
	if (c1.a==0) {
		return c2;
	}
	double aR=c1.a+c2.a*(1-c1.a);
	return ColorRgbA(
		(c1.r*c1.a+c2.r*c2.a*(1-c1.a))/aR,
		(c1.g*c1.a+c2.g*c2.a*(1-c1.a))/aR,
		(c1.b*c1.a+c2.b*c2.a*(1-c1.a))/aR,
		aR
	);
}
ColorRgbA::ColorRgbA(const ColorHsl &c):a(1) {
	if (c.s==0) {
		r=1;
		g=1;
		b=1;
		return ;
	}
	double h=c.h;
	double s=c.s;
	double l=c.l;
	double p=l<0.5?l*(1+s):l+s-l*s;
	double q=2*l-p;
	r=hue2rgb(q,p,h+1.0/3);
	g=hue2rgb(q,p,h      );
	b=hue2rgb(q,p,h-1.0/3);
}
ColorRgbA::ColorRgbA(const ColorYuv &c):a(1) {
	double y=c.y;
	double cb=c.u-0.5;
	double cr=c.v-0.5;
	r=clip(y+cr*1.402);
	g=clip(y-cr*0.714-cb*0.344);
	b=clip(y+cb*1.772);
}
ColorRgbA::ColorRgbA(const Complex &c,bool gray) {
	if(c.x==0&&c.y==0) {
		r=1;
		g=0;
		b=0;
		a=0.003;
		//return ;
	}
	else {
		r=0;
		g=0;
		b=1;
		a=0.5;
		//return ;
	}
	if (gray) {
		static ColorRgbA ret=ColorRgbA(ColorYuv(0.5,0.5,0.5));
		*this=ret;
	}
	else {
		static bool init=false;
		static ColorRgbA colorMark[]={
			ColorRgbA(0,1,0),
			ColorRgbA(0,1,1),
			ColorRgbA(0,0,1),
			ColorRgbA(1,0,1),
			ColorRgbA(1,0,0),
			ColorRgbA(1,1,0)
		};
		const int N=sizeof(colorMark)/sizeof(colorMark[0]);
		static double angleMark[N+1];
		if (!init) {
			init=true;
			int i;
			for (i=0;i<N;i++) {
				ColorYuv a(colorMark[i]);
				angleMark[i]=atan2(a.v-0.5,a.u-0.5);
				if (mpiGlobal.rank==1) {
				}
			}
			angleMark[N]=angleMark[0]+2*PI;
		}
		double p=c.angle();
		p/=2*PI;
		p-=floor(p);
		int i=p*N;
		p=p*N-i;
		if (i<0||i>=N) {
			i=0;
			p=0;
		}
		i=(i+4)%N;
		p=angleMark[i]*(1-p)+angleMark[i+1]*p;
		*this=ColorRgbA(ColorYuv(0.5,(cos(p)+1)/2,(sin(p)+1)/2));
	}
	a=c.x*c.x+c.y*c.y;
}
void ColorRgbA::writeToBytes(unsigned char *p) {
	p[0]=255*clip(r);
	p[1]=255*clip(g);
	p[2]=255*clip(b);
}
ColorYuv::ColorYuv(const ColorRgbA &c) {
	y=0.299*c.r+0.587*c.g+0.114*c.b;
	u=-0.169*c.r-0.331*c.g+0.499*c.b+0.5;
	v=0.499*c.r-0.418*c.g-0.0813*c.b+0.5;
}



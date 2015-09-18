#include<math.h>
#include<stdio.h>
#include<iostream>
#include"color.h"
double hue2rgb(double p,double q,double t) {
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
ColorHsl::ColorHsl(unsigned char h,unsigned char s,unsigned char l):h(h),s(s),l(l) {
}
ColorHsl::ColorHsl(double h,double s,double l):h(h*255),s(s*255),l(l*255) {
}
std::ostream & operator << (std::ostream & cout,const ColorHsl &c) {
	static char s[255];
	sprintf(s,"(H=%3d,S=%3d,L=%3d)",c.h,c.s,c.l);
	return std::cout<<s;
}
ColorRgb::ColorRgb(unsigned char r,unsigned char g,unsigned char b):r(r),g(b),b(b) {
}
ColorRgb::ColorRgb(double r,double g,double b):r(r*255),g(g*255),b(b*255) {
}
ColorRgb::ColorRgb(const ColorHsl &c) {
	if (c.s==0) {
		r=255;
		g=255;
		b=255;
		return ;
	}
	double h=c.h/255.0;
	double s=c.s/255.0;
	double l=c.l/255.0;
	double p=l<0.5?l*(1+s):l+s-l*s;
	double q=2*l-p;
	r=hue2rgb(q,p,h+1.0/3)*255;
	g=hue2rgb(q,p,h      )*255;
	b=hue2rgb(q,p,h-1.0/3)*255;
}
std::ostream & operator << (std::ostream & cout,const ColorRgb &c) {
	static char s[255];
	sprintf(s,"(R=%3d,G=%3d,B=%3d)",c.r,c.g,c.b);
	return std::cout<<s;
}
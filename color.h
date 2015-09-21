#pragma once
#include<iostream>
#include"geom.h"
struct ColorRgbA;
struct ColorHsl;
struct ColorRgb;
struct ColorYuv;
struct ColorRgbA {
	double r,g,b,a;
	ColorRgbA(double r=0,double g=0,double b=0,double a=0);
	ColorRgbA(const ColorHsl &c);
	ColorRgbA(const ColorYuv &c);
	ColorRgbA(const Complex &c);
	void writeToBytes(unsigned char *p);
};
ColorRgbA operator + (const ColorRgbA &c1,const ColorRgbA &c2);
struct ColorHsl {
	double h,s,l;
	ColorHsl(double h,double s,double l);
};
std::ostream & operator << (std::ostream & cout,const ColorHsl &c);
struct ColorYuv {
	double y,u,v;
	ColorYuv(double y,double u,double v);
	ColorYuv(const ColorRgbA &c);
};



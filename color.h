#pragma once
#include<iostream>
struct ColorHsl;
struct ColorRgb;
struct ColorYuv;
struct ColorHsl {
	unsigned char h,s,l;
	ColorHsl(unsigned char h,unsigned char s,unsigned char l);
	ColorHsl(double h,double s,double l);
};
std::ostream & operator << (std::ostream & cout,const ColorHsl &c);
struct ColorRgb {
	unsigned char r,g,b;
	ColorRgb(unsigned char r=0,unsigned char g=0,unsigned char b=0);
	ColorRgb(double r,double g,double b);
	ColorRgb(const ColorHsl &c);
	ColorRgb(const ColorYuv &c);
};
std::ostream & operator << (std::ostream & cout,const ColorRgb &c);
struct ColorYuv {
	unsigned char y,u,v;
	ColorYuv(unsigned char y=0,unsigned char u=0,unsigned char v=0);
	ColorYuv(double y,double u,double v);
};

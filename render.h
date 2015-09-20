#pragma once
#include<string>
#include<vector>
#include<gif_lib.h>
#include"geom.h"
class GifMaker {
public:
	GifMaker(int width,int height,int nFrame,const std::string &filename);
	~GifMaker();
	bool setFrame(int iFrame,const unsigned char *data,double delay);
private:
	bool addLoop(GifFileType *gf);
	bool save();
	std::vector< std::vector<unsigned char> > frames;
	const int width,height,nFrame;
	const std::string filename;
	unsigned char *red,*green,*blue;
	int *duration;
};
class SphericalFunctionPlotter {
public:
	SphericalFunctionPlotter(Complex (*f)(double,double,double,double),int width,int height,double zoom,int time,const std::string &filename,int fps=24);
	~SphericalFunctionPlotter();
	void addViewPort(Rect2 screen,Quaternion camera);
private:
	void plot();
	std::vector< std::pair<Rect2,Quaternion> > views;
	Complex (*f)(double r,double theta,double phi,double t);
	Vector3 zoom3;
	int width,height,depth,time;
	GifMaker gif;
	const int fps;
};

void test_render();

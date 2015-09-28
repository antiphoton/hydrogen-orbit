#pragma once
#include<string>
#include<vector>
#include<gif_lib.h>
#include<jpeglib.h>
#include"mymath.h"
#include"wave.h"
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
bool writeJpegFile(unsigned char *data,int width,int height,const char *filename,int quality=100);
class WaveFunctionPlotter {
	public:
		WaveFunctionPlotter(const BasisSet * const pWave,int width,int height,double zoom,int nFrame,double frequency,double globalFactor,const std::string &filename,int fps=24);
		~WaveFunctionPlotter();
		void addViewPort(Rect2 screen,Quaternion camera);
		void calcPixel();
		void writeStats();
		void writeJpeg();
		void makeVideo();
	private:
	const BasisSet * const pWave;
	const int height,width,depth,nFrame;
	const double frequency;
	const double globalFactor;
	std::vector< std::pair<Rect2,Quaternion> > views;
	Vector3 zoom3;
	ParallelHistogram lumaHist;
	MpiSharedArray4<unsigned char> pixelData;
	const std::string filename;
	const int fps;
};




#include"math.h"
#include<stdio.h>
#include<iostream>
#include"render.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;
GifMaker::GifMaker(int width,int height,int nFrame,const string &filename):width(width),height(height),nFrame(nFrame),filename(filename) {
	red=new unsigned char[width*height*nFrame];
	green=new unsigned char[width*height*nFrame];
	blue=new unsigned char[width*height*nFrame];
	duration=new int[nFrame];
}
GifMaker::~GifMaker() {
	save();
	delete[] red;
	delete[] green;
	delete[] blue;
	delete[] duration;
}
bool GifMaker::setFrame(int iFrame,const unsigned char *data,double delay) {
	if (iFrame<0||iFrame>=nFrame) {
		return false;
	}
	int i;
	for (i=0;i<width*height;i++) {
		red[iFrame*width*height+i]=data[i*3+0];
		green[iFrame*width*height+i]=data[i*3+1];
		blue[iFrame*width*height+i]=data[i*3+2];
	}
	duration[iFrame]=delay*100;
	return true;
}
bool GifMaker::addLoop(GifFileType *gf) {
	//return true;
	int loop_count;
	loop_count=0;
	{
		char nsle[12] = "NETSCAPE2.0";
		char subblock[3];
		if (EGifPutExtensionFirst(gf, APPLICATION_EXT_FUNC_CODE, 11, nsle) == GIF_ERROR) {
			return false;
		}
		subblock[0] = 1;
		subblock[2] = loop_count % 256;
		subblock[1] = loop_count / 256;
		if (EGifPutExtensionLast(gf, APPLICATION_EXT_FUNC_CODE, 3, subblock) == GIF_ERROR) {
			return false;
		}

	}
	return true;
}
bool GifMaker::save() {
	unsigned char *data=new unsigned char[width*height*nFrame];
	int nColor=256;
	ColorMapObject *colorMap=MakeMapObject(nColor,NULL);
	QuantizeBuffer(width,height*nFrame,&nColor,red,green,blue,data,colorMap->Colors);
	GifFileType *file=EGifOpenFileName(filename.c_str(),FALSE);
	EGifPutScreenDesc(file,width,height,nColor,0,colorMap);
	addLoop(file);
	int i;
	for (i=0;i<nFrame;i++) {
		static unsigned char ExtStr[4] = { 0x04, 0x00, 0x00, 0xff };
		ExtStr[0] = (false) ? 0x06 : 0x04;
		ExtStr[1] = duration[i] % 256;
		ExtStr[2] = duration[i] / 256;
		EGifPutExtension(file, GRAPHICS_EXT_FUNC_CODE, 4, ExtStr);
		EGifPutImageDesc(file,0,0,width,height,FALSE,NULL);
		int y;
		for (y=0;y<height;y++) {
			EGifPutLine(file,&data[i*width*height+y*width],width);
		}
	}
	EGifCloseFile(file);
	return true;
}

void test_render() {
	const int PI=acos(-1);
	const int w=800,h=600,l=24;
	GifMaker gif(w,h,l,"/home/cbx/Dropbox/nodejs/web/buffer/output.gif");
	static unsigned char *data=new unsigned char[w*h*3];
	int y,x,t;
	int z;
	for (t=0;t<l;t++) {
		for (y=0;y<h;y++) {
			for (x=0;x<w;x++) {
				z=255*sin(1.0*y/h*2*PI)*sin(1.0*x/w*2*PI)*cos(1.0*t/l*2*PI);
				data[(y*w+x)*3]=z>=0?z:0;
				data[(y*w+x)*3+1]=0;
				data[(y*w+x)*3+2]=z<0?-z:0;
			}
		}
		gif.setFrame(t,data,1.0/24);
	}
	delete[] data;
}


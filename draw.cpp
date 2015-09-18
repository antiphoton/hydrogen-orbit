#include<math.h>
#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include<vector>
#include<algorithm>
#include"jpeglib.h"

#include"color.h"
#include"geom.h"
#include"draw.h"
const double PI=acos(-1.0);
const int QUALITY=95;
const int CANVAS_WIDTH=600,CANVAS_HEIGHT=600;
static unsigned char canvas[CANVAS_HEIGHT][CANVAS_WIDTH*3];
Quaternion camera(Vector3(0.732050807,1.767326988,3.414213562),acos(-1.0)/180*220);
double viewRangeX=1,viewRangeY=viewRangeX/CANVAS_WIDTH*CANVAS_HEIGHT;
struct BufferedPixel {
	double z;
	Complex v;
	BufferedPixel(double z,const Complex &v):z(z),v(v) {
	};
};
bool operator < (const BufferedPixel &p1,const BufferedPixel &p2) {
	return p1.z<p2.z;
}
std::vector<BufferedPixel> buffer[CANVAS_HEIGHT][CANVAS_WIDTH];
void clearBuffer() {
	int y,x;
	for (y=0;y<CANVAS_HEIGHT;y++) {
		for (x=0;x<CANVAS_WIDTH;x++) {
			buffer[y][x].clear();
			canvas[y][x*3+0]=255;
			canvas[y][x*3+1]=255;
			canvas[y][x*3+2]=255;
		}
	}
};
void appendBuffer(int x,int y,double z,const Complex &v) {
	if (y<0||y>=CANVAS_HEIGHT||x<0||x>=CANVAS_WIDTH) {
		return ;
	}
	buffer[y][x].push_back(BufferedPixel(z,v));
};
void finishBuffer() {
	double maxP=0;
	int y,x;
	for (y=0;y<CANVAS_HEIGHT;y++) {
		for (x=0;x<CANVAS_WIDTH;x++) {
			std::sort(buffer[y][x].begin(),buffer[y][x].end());
			std::vector<BufferedPixel> &v=buffer[y][x];
			int n=v.size();
			int i;
			for (i=0;i<n;i++) {
				double l=v[i].v.length();
				l*=l;
				if (l>maxP) {
					maxP=l;
				}
			}
		}
	}
	for (y=0;y<CANVAS_HEIGHT;y++) {
		for (x=0;x<CANVAS_WIDTH;x++) {
			std::vector<BufferedPixel> &v=buffer[y][x];
			unsigned char &r=canvas[y][x*3+0];
			unsigned char &g=canvas[y][x*3+1];
			unsigned char &b=canvas[y][x*3+2];
			int n=v.size();
			int i;
			for (i=0;i<n;i++) {
				BufferedPixel &p=v[i];
				double l=p.v.length();
				l=l*l/maxP;
				ColorRgb c(ColorHsl(p.v.angle()/PI/2,1,0.5));
				double dz=i>0?v[i].z-v[i-1].z:100;
				if (dz>1) {
					dz=1;
				}
				unsigned char a1=(1-exp(-l*dz))*255;
				unsigned char a0=255-a1;
				r=r*a0/255+c.r*a1/255;
				g=g*a0/255+c.g*a1/255;
				b=b*a0/255+c.b*a1/255;
			}
		}
	}
};
void setPoint(const Vector3 &p0,const Complex &v) {
	Vector3 p1=camera.rotate(p0);
	appendBuffer((p1.x/viewRangeX+1)/2*CANVAS_WIDTH,(1-p1.y/viewRangeY)/2*CANVAS_HEIGHT,p1.z,v);
}
void writeJpeg() {
	const int LENGTH=6;
	static int imageId=0;
	static char fileNameFormat[255];
	sprintf(fileNameFormat,"/home/cbx/Dropbox/nodejs/web/buffer/%%0%dd.jpg",LENGTH);
	static char filename[255];
	sprintf(filename,fileNameFormat,imageId);
	imageId++;
	FILE *file=fopen(filename,"wb");
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, file);

	cinfo.image_width      = CANVAS_WIDTH;
	cinfo.image_height     = CANVAS_HEIGHT;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo,QUALITY,true);
	jpeg_start_compress(&cinfo,true);
	static JSAMPROW row[CANVAS_HEIGHT];
	int i;
	for (i=0;i<CANVAS_HEIGHT;i++) {
		row[i]=(JSAMPROW) canvas[i];
	}
	jpeg_write_scanlines(&cinfo,row,CANVAS_HEIGHT);
	jpeg_finish_compress(&cinfo);
}
void plotCartesianFunction(Complex (*f)(double,double,double)) {
	const double STEP=0.01;
	clearBuffer();
	double x,y,z;
	for (x=-1;x<1;x+=STEP) {
		for (y=-1;y<1;y+=STEP) {
			for (z=-1;z<1;z+=STEP) {
				setPoint(Vector3(x,y,z),f(x,y,z));
			}
		}
	}
	finishBuffer();
	writeJpeg();
}
void plotAxies() {
	clearBuffer();
	double i;
	for (i=0;i<1;i+=0.00001) {
		setPoint(Vector3(i,0,0),Complex(1,0));
		setPoint(Vector3(0,i,0),Complex(-0.5,0.866));
		setPoint(Vector3(0,0,i),Complex(-0.5,-0.866));
	}
	finishBuffer();
	writeJpeg();
}
void plotSphericalFunction(Complex (*f)(double,double,double)) {
	const double RANGE=3;
	const double STEP=RANGE/30;
	clearBuffer();
	double x,y,z;
	double r,t,p;
	for (x=-RANGE;x<RANGE;x+=STEP) {
		for (y=-RANGE;y<RANGE;y+=STEP) {
			for (z=-RANGE;z<RANGE;z+=STEP) {
				r=sqrt(x*x+y*y+z*z);
				t=acos(z/r);
				p=atan2(y,x);
				setPoint(Vector3(x,y,z),f(r,t,p));
			}
		}
	}
	finishBuffer();
	writeJpeg();
}
Complex f1(double r,double theta,double phi) {
	if (r<1) {
		return Complex(sin(theta)*cos(3*phi)/2,0);
	}
	else {
		return Complex(0,0);
	}
}
int test_draw() {
	plotAxies();
	return 0;
}


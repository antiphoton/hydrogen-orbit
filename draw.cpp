#include<math.h>
#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include<vector>
#include<algorithm>
#include"jpeglib.h"

#include"draw.h"
#include"geom.h"
const int QUALITY=75;
const int WIDTH=800,HEIGHT=600;
static unsigned char canvas[HEIGHT][WIDTH*3];
double projectionMatrix[9]={1,0,0,0,1,0,0,0,1};
static inline Vector3 getProjection(const Vector3 &p) {
	return Vector3(projectionMatrix[0]*p.x+projectionMatrix[1]*p.y+projectionMatrix[2]*p.z,projectionMatrix[3]*p.x+projectionMatrix[4]*p.y+projectionMatrix[5]*p.z,projectionMatrix[6]*p.x+projectionMatrix[7]*p.y+projectionMatrix[8]*p.z);
}
void setCamera(const Vector3 &p) {
}
struct BufferedPixel {
	double z;
	unsigned r,g,b,a;
};
bool operator < (const BufferedPixel &p1,const BufferedPixel &p2) {
	return p1.z<p2.z;
}
std::vector<BufferedPixel> buffer[HEIGHT][WIDTH];
void clearBuffer() {
	int y,x;
	for (y=0;y<HEIGHT;y++) {
		for (x=0;x<WIDTH;x++) {
			buffer[y][x].clear();
			canvas[y][x*3+0]=255;
			canvas[y][x*3+1]=255;
			canvas[y][x*3+2]=255;
		}
	}
};
void appendBuffer(int y,int x,double z,unsigned char r,unsigned char g,unsigned char b,unsigned char a) {
	if (y<0||y>=HEIGHT||x<0||x>=WIDTH) {
		return ;
	}
	BufferedPixel p;
	p.z=z;
	p.r=r;
	p.g=g;
	p.b=b;
	p.a=a;
	buffer[y][x].push_back(p);
};
void finishBuffer() {
	int y,x;
	for (y=0;y<HEIGHT;y++) {
		for (x=0;x<WIDTH;x++) {
			std::sort(buffer[y][x].begin(),buffer[y][x].end());
			std::vector<BufferedPixel> &v=buffer[y][x];
			unsigned char &r=canvas[y][x*3+0];
			unsigned char &g=canvas[y][x*3+1];
			unsigned char &b=canvas[y][x*3+2];
			int n=v.size();
			int i;
			for (i=0;i<n;i++) {
				BufferedPixel &p=v[i];
				unsigned char a1=p.a;
				unsigned char a0=255-a1;
				r=r*a0/255+p.r*a1/255;
				g=g*a0/255+p.g*a1/255;
				b=b*a0/255+p.b*a1/255;
			}
		}
	}
};
void setPoint(const Vector3 &p0,double real,double imaginary=0) {
	Vector3 p1=getProjection(p0);
	appendBuffer(p1.x+WIDTH/2,p1.y+HEIGHT/2,p1.z,255-255*real,255-255*real,255-255*real,255);
}
void writeJpeg() {
	const int LENGTH=6;
	static int imageId=0;
	static char fileNameFormat[255];
	sprintf(fileNameFormat,"output/%%0%dd.jpg",LENGTH);
	static char filename[255];
	sprintf(filename,fileNameFormat,imageId);
	imageId++;
	FILE *file=fopen(filename,"wb");
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, file);

	cinfo.image_width      = WIDTH;//WIDTH;
	cinfo.image_height     = HEIGHT;//HEIGHT;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo,QUALITY,true);
	jpeg_start_compress(&cinfo,true);
	static JSAMPROW row[HEIGHT];
	int i;
	for (i=0;i<HEIGHT;i++) {
		row[i]=(JSAMPROW) canvas[i];
	}
	jpeg_write_scanlines(&cinfo,row,HEIGHT);
	jpeg_finish_compress(&cinfo);
}
int test_draw() {
	clearBuffer();
	int x;
	for (x=-100;x<100;x++) {
		setPoint(Vector3(x/180.0*3.1416*100,sin(x)*300,0),1);
	}
	finishBuffer();
	writeJpeg();
	return 0;
}


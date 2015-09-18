#include<math.h>
#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include<vector>
#include<algorithm>
#include"jpeglib.h"

#include"color.h"
#include"draw.h"
#include"geom.h"
const double PI=acos(-1.0);
const int QUALITY=95;
const int CANVAS_WIDTH=600,CANVAS_HEIGHT=600;
static unsigned char canvas[CANVAS_HEIGHT][CANVAS_WIDTH*3];
Quaternion camera(Vector3(1,1,1),acos(-1.0)/180*30);
double viewRangeX=1,viewRangeY=viewRangeX/CANVAS_WIDTH*CANVAS_HEIGHT;
struct BufferedPixel {
	double z;
	ColorRgb color;
	unsigned char alpha;
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
void appendBuffer(int x,int y,double z,const ColorRgb &color,unsigned char a) {
	if (y<0||y>=CANVAS_HEIGHT||x<0||x>=CANVAS_WIDTH) {
		return ;
	}
	BufferedPixel p;
	p.z=z;
	p.color=color;
	p.alpha=a;
	buffer[y][x].push_back(p);
};
void finishBuffer() {
	int y,x;
	for (y=0;y<CANVAS_HEIGHT;y++) {
		for (x=0;x<CANVAS_WIDTH;x++) {
			std::sort(buffer[y][x].begin(),buffer[y][x].end());
			std::vector<BufferedPixel> &v=buffer[y][x];
			unsigned char &r=canvas[y][x*3+0];
			unsigned char &g=canvas[y][x*3+1];
			unsigned char &b=canvas[y][x*3+2];
			int n=v.size();
			int i;
			for (i=0;i<n;i++) {
				BufferedPixel &p=v[i];
				unsigned char a1=p.alpha;
				unsigned char a0=255-a1;
				r=r*a0/255+p.color.r*a1/255;
				g=g*a0/255+p.color.g*a1/255;
				b=b*a0/255+p.color.b*a1/255;
			}
		}
	}
};
void setPoint(const Vector3 &p0,double real,double imaginary=0) {
	Vector3 p1=camera.rotate(p0);
	double m=sqrt(real*real+imaginary*imaginary);
	double t=atan2(imaginary,real);
	if (m>1) {
		m=1;
	}
	//printf("%f\n",t/PI/2);
	//printf("%f\n",(t/PI+1)/2);
	appendBuffer((p1.x/viewRangeX+1)/2*CANVAS_WIDTH,(1-p1.y/viewRangeY)/2*CANVAS_HEIGHT,p1.z,ColorRgb(ColorHsl(t/PI/2,1,0.5)),m*255);
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
int test_draw() {
	clearBuffer();
	double i;
	for (i=-0;i<1;i+=0.001) {
		setPoint(Vector3(i,0,0),1,0);
		setPoint(Vector3(0,i,0),-0.5,0.866);
		setPoint(Vector3(0,0,i),-0.5,-0.866);
	}
	finishBuffer();
	writeJpeg();
	return 0;
}


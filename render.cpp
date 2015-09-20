#include"math.h"
#include<stdio.h>
#include<iostream>
#include"render.h"
#include"color.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::make_pair;
const double PI=acos(-1.0);
const int FPS=30;
GifMaker::GifMaker(int width,int height,int nFrame,const string &filename):width(width),height(height),nFrame(nFrame),filename(filename) {
	red=new unsigned char[width*height*nFrame];
	green=new unsigned char[width*height*nFrame];
	blue=new unsigned char[width*height*nFrame];
	duration=new int[nFrame];
	int i;
	for (i=0;i<nFrame;i++) {
		duration[i]=1;
	}
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
SphericalFunctionPlotter::SphericalFunctionPlotter(Complex (*f)(double,double,double,double),int width,int height,double zoom,int time,const string &filename,int fps):f(f),width(width),height(height),time(time),gif(width,height,time,filename),fps(fps) {
	if (width<=height) {
		zoom3=Vector3(zoom,zoom/height*width,0);
	}
	else {
		zoom3=Vector3(zoom/width*height,zoom,0);
	}
	zoom3.z=sqrt(zoom3.x*zoom3.y);
	depth=sqrt(width*height);
}
SphericalFunctionPlotter::~SphericalFunctionPlotter() {
	if (views.size()==0) {
		addViewPort(Rect2(0,0,1,1),Quaternion(Vector3(0.732050807,1.767326988,3.414213562),acos(-1.0)/180*220));
	}
	plot();
}
void SphericalFunctionPlotter::addViewPort(Rect2 screen,Quaternion camera) {
	if (screen.min.x<0) {
		screen.min.x=0;
	}
	if (screen.min.y<0) {
		screen.min.y=0;
	}
	if (screen.max.x>1) {
		screen.max.x=1;
	}
	if (screen.max.y>1) {
		screen.max.y=1;
	}
	if (screen.min.x>=screen.max.x) {
		return ;
	}
	if (screen.min.y>=screen.max.y) {
		return ;
	}
	views.push_back(make_pair(screen,-camera));
}
void SphericalFunctionPlotter::plot() {
	unsigned char *data=new unsigned char[width*height*3];
	int t;
	for (t=0;t<time;t++) {
		int y2,x2,z2;
		for (y2=0;y2<height;y2++) {
			for (x2=0;x2<width;x2++) {
				double r=1,g=1,b=1;
				int iView;
				for (iView=views.size()-1;iView>=0;iView--) {
					Vector2 p1=views[iView].first.screenToClient(Vector2(1.0*x2/width,1.0*y2/height));
					if (p1.x<0||p1.x>=1) {
						continue;
					}
					if (p1.y<0||p1.y>=1) {
						continue;
					}
					for (z2=0;z2<depth;z2++) {
						Vector3 p(p1.x-0.5,0.5-p1.y,1.0*z2/depth-0.5);
						p.x/=zoom3.x;
						p.y/=zoom3.y;
						p.z/=zoom3.z;
						p=views[iView].second.rotate(p);
						double rho,theta,phi;
						rho=p.length();
						if (rho>0) {
							theta=acos(p.z/rho);
							phi=atan2(p.y,p.x);
						}
						else {
							theta=0;
							phi=0;
						}
						Complex value=f(rho,theta,phi,t);
						ColorRgb color(ColorHsl(value.angle()/PI/2,1,0.5));
						double alpha=value.length();
						alpha*=alpha;
						if (alpha>1) {
							alpha=1;
						}
						r=r*(1-alpha)+color.r/255.0*alpha;
						g=g*(1-alpha)+color.g/255.0*alpha;
						b=b*(1-alpha)+color.b/255.0*alpha;
					}
				}
				data[(y2*width+x2)*3+0]=r>1?255:255*r;
				data[(y2*width+x2)*3+1]=g>1?255:255*g;
				data[(y2*width+x2)*3+2]=b>1?255:255*b;
			}
		}
		gif.setFrame(t,data,1.0/fps);
	}
	delete[] data;
}

Complex f1(double r,double theta,double phi,double t) {
	const double omega=2*PI/144;
	Complex ret(0,0);
	double x=r*sin(theta)*cos(phi);
	double y=r*sin(theta)*sin(phi);
	double z=r*cos(theta);
	if (fabs(y)<0.1&&fabs(z)<0.1&&x>0&&x<2) {
		ret=Complex(1,0);
	}
	if (fabs(z)<0.1&&fabs(x)<0.1&&y>0&&y<2) {
		ret=Complex(-0.5,0.866);
	}
	if (fabs(x)<0.1&&fabs(y)<0.1&&z>0&&z<2) {
		ret=Complex(-0.5,-0.866);
	}
	ret=Complex(sin(theta)*cos(phi),sin(theta)*sin(phi))*exp(-0.5*r*r);
	return ret*Complex(cos(t*omega),sin(t*omega));
	t=2;
	if (r<1) {
	}
	else {
		return Complex(0,0);
	}
}

void test_render() {
	const int w=50,h=50,l=144;
	//const int w=10,h=10,d=10,l=1;
	SphericalFunctionPlotter sp(f1,w,h,0.2,l,"/home/cbx/Dropbox/nodejs/web/buffer/output.gif");
	//sp.addViewPort(Rect2(0,0,1,1),Quaternion(Vector3(0.732050807,1.767326988,3.414213562),acos(-1.0)/180*220));
}


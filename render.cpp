#include<math.h>
#include<string.h>
#include<stdio.h>
#include<cstdlib>
#include<ctime>
#include<iostream>
#include"mympi.h"
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
bool writeJpegFile(unsigned char *data,int width,int height,const char *filename,int quality) {
	FILE *file=fopen(filename,"wb");
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, file);

	cinfo.image_width      = width;
	cinfo.image_height     = height;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo,quality,true);
	jpeg_start_compress(&cinfo,true);
	static JSAMPROW row[1000];
	int i;
	for (i=0;i<height;i++) {
		row[i]=(JSAMPROW) &data[width*i*3];
	}
	jpeg_write_scanlines(&cinfo,row,height);
	jpeg_finish_compress(&cinfo);
	fclose(file);

	return true;
}
SphericalFunctionPlotter::SphericalFunctionPlotter(Complex (*f)(double,double,double,double),int width,int height,double zoom,int nFrame,const string &filename,const string &filetype,int fps):f(f),width(width),height(height),nFrame(nFrame),filename(filename),isGif(filetype=="gif"),isJpeg(filetype=="jpeg"),fps(fps) {
	if (width<=height) {
		zoom3=Vector3(zoom,zoom/height*width,0);
	}
	else {
		zoom3=Vector3(zoom/width*height,zoom,0);
	}
	zoom3.z=sqrt(zoom3.x*zoom3.y);
	depth=sqrt(width*height);
	if (isGif) {
		gif=new GifMaker(width,height,nFrame,filename);
	}
	if (isJpeg) {
		lumaStats=new long[LUMA_WIDTH];
		memset(lumaStats,0,sizeof(long)*LUMA_WIDTH);
	}
}
SphericalFunctionPlotter::~SphericalFunctionPlotter() {
	if (views.size()==0) {
		addViewPort(Rect2(0,0,0.5,0.5),Quaternion(Vector3(0,1,1),acos(-1.0)/180*180));
		addViewPort(Rect2(0,0.5,0.5,1),Quaternion(Vector3(0,0,1),acos(-1.0)/180*180));
		addViewPort(Rect2(0.5,0,1,0.5),Quaternion(Vector3(1,1,1),acos(-1.0)/180*240));
		addViewPort(Rect2(0.5,0.5,1,1),Quaternion(Vector3(0.732050807,1.767326988,3.414213562),acos(-1.0)/180*220));
	}
	plot();
	writeStats();
	if (isGif) {
		delete gif;
	}
	if (isJpeg) {
		if (mpiGlobal.rank==0) {
			static char cmd[1024];
			sprintf(cmd,"ffmpeg -y -framerate %d -i output/img_%%05d.jpg -c:v libx264 -r 30 -pix_fmt yuv420p %s 1>/dev/null 2>&1",fps,filename.c_str());
			std::system(cmd);
			system("rm output/*.jpg");
		}
		delete[] lumaStats;
	}
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
	MpiTaskManager taskManager;
	int t;
	for (t=0;t<nFrame;t++) {
		int y2,x2,z2;
		for (y2=0;y2<height;y2++) {
			if (taskManager.isMyTask(t)) {
				for (x2=1;x2<width;x2++) {
					ColorRgbA color(1,1,1,1);
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
							color=ColorRgbA(value)+color;
							double lumaD=value.length()*value.length();
							int lumaI;
							if (lumaD<=0) {
								lumaI=0;
							}
							else {
								const double log10=log(10);
								const double logMin=log(LUMA_MIN)/log10;
								const double logMax=log(LUMA_MAX)/log10;
								lumaD=log(lumaD)/log10;
								lumaI=(lumaD-logMin)/(logMax-logMin)*LUMA_WIDTH;
								if (lumaI<0) {
									lumaI=0;
								}
								if (lumaI>=LUMA_WIDTH) {
									lumaI=LUMA_WIDTH-1;
								}
							}
							lumaStats[lumaI]++;
						}
					}
					data[(y2*width+x2)*3+0]=color.r*255;
					data[(y2*width+x2)*3+1]=color.g*255;
					data[(y2*width+x2)*3+2]=color.b*255;
				}
			}
			taskManager.submitSlowTask(t);
		}
		if (isGif) {
			gif->setFrame(t,data,1.0/fps);
		}
		if (isJpeg) {
			if (taskManager.isMyTask(t)) {
				static char fileFinalName[256];
				sprintf(fileFinalName,"output/img_%05d.jpg",t);
				writeJpegFile(data,width,height,fileFinalName,100);
			}
			taskManager.submitQuickTask(t);
		}
	}
	delete[] data;
}
void SphericalFunctionPlotter::writeStats() {
	const int TAG_STATS=10;
	if (mpiGlobal.rank!=0) {
		MPI_Send(lumaStats,LUMA_WIDTH,MPI_LONG,0,TAG_STATS,MPI_COMM_WORLD);
	}
	else {
		MPI_Status mpiStat;
		int i,j;
		static long buffer[LUMA_WIDTH];
		for (i=1;i<mpiGlobal.size;i++) {
			MPI_Recv(&buffer,LUMA_WIDTH,MPI_LONG,MPI_ANY_SOURCE,TAG_STATS,MPI_COMM_WORLD,&mpiStat);
			for (j=0;j<LUMA_WIDTH;j++) {
				lumaStats[j]+=buffer[j];
			}
		}
		const double log10=log(10);
		const double logMin=log(LUMA_MIN)/log10;
		const double logMax=log(LUMA_MAX)/log10;
		long total=0;
		double sum1=0,sum2=0;
		for (i=0;i<LUMA_WIDTH;i++) {
		}
		for (i=0;i<LUMA_WIDTH;i++) {
			double x=exp(((i+0.5)/LUMA_WIDTH*(logMax-logMin)+logMin)*log10);
			total+=lumaStats[i];
			sum1+=x*lumaStats[i];
			sum2+=x*x*lumaStats[i];
		}
		double mu=sum1/total;
		double sigma=sqrt(sum2/total-mu*mu);
		mu*=depth;
		sigma/=sqrt(depth);
		puts("Recommended factor");
		for (i=0;i<3;i++) {
			printf("%8.3f%c",sqrt((i+1)/(mu+sigma)),i<3-1?'\t':'\n');
		}
	}
}

static Complex f1(double r,double theta,double phi,double t) {
	const double omega=2*PI/216;
	Complex ret(0,0);
	//ret=Complex(sin(theta)*cos(phi),sin(theta)*sin(phi))*exp(-0.5*r*r);
	ret=Complex(cos(2*phi),sin(2*phi))*(1.0/25*r*r*exp(-r/3)*sin(theta)*sin(theta));
	return ret*Complex(cos(t*omega),sin(t*omega));
	t=2;
	if (r<1) {
	}
	else {
		return Complex(0,0);
	}
}

void test_render() {
	return ;
	//const int w=100,h=100,l=72;
	const int w=100,h=100,l=8;
	SphericalFunctionPlotter sp(f1,w,h,0.02,l,"/home/cbx/Dropbox/nodejs/web/buffer/out320.mp4","jpeg");
}


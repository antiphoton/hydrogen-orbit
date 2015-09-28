#include<math.h>
#include<string.h>
#include<stdio.h>
#include<algorithm>
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


WaveFunctionPlotter::WaveFunctionPlotter(const BasisSet * const pWave,int width,int height,double zoom,int nFrame,double frequency,double globalFactor,const std::string &filename,int fps):pWave(pWave),height(height),width(width),depth(sqrt(height*width)),nFrame(nFrame),frequency(frequency),globalFactor(globalFactor),lumaHist(true,true,1e-10,1e10,1920,"output/luma.txt"),pixelData(nFrame,height,width,3),filename(filename),fps(fps) {
	if (width<=height) {
		zoom3=Vector3(zoom,zoom/height*width,0);
	}
	else {
		zoom3=Vector3(zoom/width*height,zoom,0);
	}
	zoom3.z=sqrt(zoom3.x*zoom3.y);
	system("rm output/*.jpg 2>/dev/null");
}
WaveFunctionPlotter::~WaveFunctionPlotter() {
	if (views.size()==0) {
		addViewPort(Rect2(0,0,0.5,0.5),Quaternion(Vector3(0,1,1),acos(-1.0)/180*180));
		addViewPort(Rect2(0,0.5,0.5,1),Quaternion(Vector3(0,0,1),acos(-1.0)/180*180));
		addViewPort(Rect2(0.5,0,1,0.5),Quaternion(Vector3(1,1,1),acos(-1.0)/180*240));
		addViewPort(Rect2(0.5,0.5,1,1),Quaternion(Vector3(0.732050807,1.767326988,3.414213562),acos(-1.0)/180*220));
	}
	calcPixel();
	SINGLERUN();
	writeStats();
	SINGLERUN();
	writeJpeg();
	SINGLERUN();
	makeVideo();
}
void WaveFunctionPlotter::addViewPort(Rect2 screen,Quaternion camera) {
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
void WaveFunctionPlotter::calcPixel() {
	using std::random_shuffle;
	using std::pair;
	using std::make_pair;
	MpiSharedArray< pair<int,int> > pixelList(width*height);
	SINGLERUN(
		int y;
		int x;
		for (y=0;y<height;y++) {
			for (x=0;x<width;x++) {
				pixelList[y*width+x]=make_pair(y,x);
			}
		}
	);
	int nWave=pWave->getSize();
	MpiSharedArray2<Complex> timeFactor(nFrame,nWave);
	SINGLERUN(
		double *pEnergy=new double[nWave];
		pWave->getEnergy(pEnergy);
		int iFrame;
		int iWave;
		for (iFrame=0;iFrame<nFrame;iFrame++) {
			double time=1.0*iFrame/fps*frequency*2*PI;
			for (iWave=0;iWave<nWave;iWave++) {
				*(timeFactor.address(iFrame,iWave))=Complex(cos(pEnergy[iWave]*time),sin(pEnergy[iWave]*time));
			}
		}
		delete[] pEnergy;
	);
	Complex *waveValue=new Complex[depth*nWave];
	int iPixel;
	for (iPixel=0;iPixel<width*height;iPixel++) {
		if (!mpiGlobal.myDuty(iPixel)) {
			continue;
		}
		int yCanvas=pixelList[iPixel].first;
		int xCanvas=pixelList[iPixel].second;
		int iView;
		for (iView=views.size()-1;iView>=0;iView--) {
			Vector2 pClient=views[iView].first.screenToClient(Vector2(1.0*xCanvas/width,1.0*yCanvas/height));
			if (pClient.x<0||pClient.x>=1) {
				continue;
			}
			if (pClient.y<0||pClient.y>=1) {
				continue;
			}
			int zI;
			for (zI=0;zI<depth;zI++) {
				Vector3 pWorld(pClient.x-0.5,pClient.y-0.5,1.0*zI/depth-0.5);
				pWorld.x/=zoom3.x;
				pWorld.y/=zoom3.y;
				pWorld.z/=zoom3.z;
				pWorld=views[iView].second.rotate(pWorld);
				pWave->getValueByCartesian(waveValue+zI*nWave,pWorld);
			}
			int iFrame;
			for (iFrame=0;iFrame<nFrame;iFrame++) {
				ColorRgbA color(1,1,1,1);
				for (zI=0;zI<depth;zI++) {
					int iWave;
					Complex valueNow(0,0);
					for (iWave=0;iWave<nWave;iWave++) {
						valueNow+=waveValue[zI*nWave+iWave]*(*(timeFactor.address(iFrame,iWave)));
					}
					valueNow=valueNow*globalFactor;
					color=ColorRgbA(valueNow)+color;
					lumaHist.tip(valueNow.lengthSqr());
				}
				unsigned char *p0=pixelData.address(iFrame,yCanvas,xCanvas,0);
				*(p0+0)=color.r*255;
				*(p0+1)=color.g*255;
				*(p0+2)=color.b*255;
			}
			break;
		}
	}
	delete[] waveValue;
}
void WaveFunctionPlotter::writeStats() {
	lumaHist.writeRecommand(depth);
}
void WaveFunctionPlotter::writeJpeg() {
	SINGLERUN(
		system("rm output/*.jpg 2>/dev/null");
	);
	char *pFileFinalName=new char[256];
	int iFrame;
	for (iFrame=0;iFrame<nFrame;iFrame++) {
		if (!mpiGlobal.myDuty(iFrame)) {
			continue;
		}
		sprintf(pFileFinalName,"output/img_%05d.jpg",iFrame);
		writeJpegFile(pixelData.address(iFrame,0,0,0),width,height,pFileFinalName,100);
	}
	delete pFileFinalName;
}
void WaveFunctionPlotter::makeVideo() {
	SINGLERUN(
		char *cmd=new char[1024];
		sprintf(cmd,"ffmpeg -y -framerate %d -i output/img_%%05d.jpg -c:v libx264 -r 30 -pix_fmt yuv420p %s 1>/dev/null 2>&1",fps,filename.c_str());
		std::system(cmd);
		delete[] cmd;
	)
}


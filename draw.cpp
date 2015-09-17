#include<stdio.h>
#include<cstdlib>
#include"jpeglib.h"
#include"draw.h"
const int QUALITY=75;
const int WIDTH=800,HEIGHT=600;
unsigned char canvas[HEIGHT][WIDTH*3];
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
	int i,j;
	for (i=0;i<HEIGHT;i++) {
		for (j=0;j<WIDTH*3;j++) {
			canvas[i][j]=rand()%255;
		}
	}
	writeJpeg();
	writeJpeg();
	writeJpeg();
	return 0;
}


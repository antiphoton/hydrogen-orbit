#include<string>
#include<vector>
#include"gif_lib.h"
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
void test_render();

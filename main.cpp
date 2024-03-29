#include<math.h>
#include<stdio.h>
#include<mpi.h>
#include<unistd.h>
#include<algorithm>
#include<fstream>
#include<iostream>
#include<vector>
#include"mymath.h"
#include"mympi.h"
#include"render.h"
#include"monte_carlo.h"
#include"wave.h"
using std::cout;
using std::endl;
using std::make_pair;
using std::ostream;
using std::pair;
using std::vector;
int main(int argc, char **argv) {
	using namespace std;
	BasisSet wave(4);
	wave.project(sommerfeld(2));
	wave.writeWeight();
	WaveFunctionPlotter plotter(&wave,
		500,//width
		500,//height
		0.05,//zoom
		720,//nFrame
		0.5,//frequency
		7,//globalFactor
		true,//gray
		"output/out.mp4");
	return 0;
};


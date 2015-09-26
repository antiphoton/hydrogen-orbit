#pragma once
#include"mymath.h"
Complex integrateSqrtNormal3(Complex (*f)(double r,double theta,double phi),const WavePacket &wave,double eps=5e-5);


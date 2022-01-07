#pragma once

#include <functional>
#include <complex>
#include "image.h"

struct MandelbrotParams {
	double x_start = -2.1;
	double y_start = -1.2;
	double x_range = 2.8;
	double y_range = 2.4;
};


struct MColor { float r, g, b; };

//MColor Mandelbrot_Pixel(std::complex<double> c);
void Mandelbrot_Image(MandelbrotParams p,int width, int height, std::function<void(int, int, const Image::Colour&)>&& pixel);
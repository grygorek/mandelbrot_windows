/// Copyright 2022 Piotr Grygorczuk <grygorek@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.

#include <functional>
#include <complex>
#include <thread>
#include <future>
#include <string>
#include <algorithm>
#include "image.h"
#include "mandel_algo.h"
#include "debug_output.h"
#include "hsv.h"

template <class T>
T my_max(T l, T r) {
	return l < r ? r : l;
}

using X = std::vector<int>;
std::vector<X> counts;

int Mandelbrot_Pixel(std::complex<double> c)
{
	std::complex<double> z{};
	int count = 0;
	int depth = 2000;
	while (std::abs(z) <= 2. && count++ < depth)
	{
		z = z * z + c;
	}

	return count;
}

int Loop(double x_start, double y_start, double stepx, double stepy,
	int x_pos_begin, int x_pos_end, int height)
{
	int max = 0;
	double x0 = x_start;
	for (int x = x_pos_begin; x < x_pos_end; x++)
	{
		double y0 = y_start;
		for (int y = 0; y < height; y++)
		{
			auto c = Mandelbrot_Pixel({ x0, y0 });
			counts[x][y] = c;
			max = my_max(c, max);
			y0 += stepy;
		}
		x0 += stepx;
	}

	return max;
}


void Mandelbrot_Image(MandelbrotParams p, int width, int height, std::function<void(int, int, const Image::Colour&)>&& pixel) 
{

	counts.resize(width);
	std::for_each(counts.begin(), counts.end(), [height](auto& v) {v.resize(height); });

	double stepx = p.x_range / width;
	double stepy = p.y_range / height;

	auto t0 = std::async(std::launch::async, [xs = p.x_start, ys = p.y_start, stepx, stepy, xstart = 0, xend = width / 4, height]{
		return Loop(xs, ys, stepx, stepy, xstart, xend, height);
		});


	p.x_start += width / 4 * stepx;
	auto t1 = std::async(std::launch::async, [xs = p.x_start, ys = p.y_start, stepx, stepy, xstart = width / 4, xend = width / 2, height]{
		return Loop(xs, ys, stepx, stepy, xstart, xend, height);
		});

	p.x_start += width / 4 * stepx;
	auto t2 = std::async(std::launch::async, [xs = p.x_start, ys = p.y_start, stepx, stepy, xstart = width / 2, xend = 3 * width / 4, height]{
		return Loop(xs, ys, stepx, stepy, xstart, xend, height);
		});


	p.x_start += width / 4 * stepx;
	int max = Loop(p.x_start, p.y_start, stepx, stepy, 3 * width / 4, width, height);

	max = my_max(t0.get(), max);
	max = my_max(t1.get(), max);
	max = my_max(t2.get(), max);

	int total = 0;
	std::vector<int> count_per_pix;
	count_per_pix.resize(max + 1);
	for (const auto& v : counts)
	{
		for (const int c : v)
		{
			++count_per_pix[c];
			total += c;
		}
	}

	const float totalf = total;

	std::vector<std::vector<float>> hue;
	hue.resize(width);
	std::for_each(hue.begin(), hue.end(), [height](auto& v) {v.resize(height); });
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int count = counts[x][y];
			for (int i = 0; i < count; i++)
			{
				hue[x][y] += count_per_pix[i] / totalf;
			}

			rgb c = hsv2rgb({ hue[x][y], 255, counts[x][y] < max ? 255. : 0 });
			pixel(x, y, { (float)c.r, (float)c.g, (float)c.b });

		}
	}
}
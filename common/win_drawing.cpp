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

#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>
#pragma comment(lib, "Gdiplus.lib")

#include "image.h"
#include <memory>

void DrawImage(Gdiplus::Bitmap &hnd, const Image &image)
{
  int width{0};
  int height{0};
  for (const auto &pixel : image)
  {
    if (width >= image.width)
    {
      width = 0;
      height++;
    }
    hnd.SetPixel(width++, height, Gdiplus::Color(0xFF000000 | Colour(pixel, false)));
  }
}

std::unique_ptr<Gdiplus::Bitmap> g_bitmap;

void OnPaint(HDC &hnd, Image &image)
{
  Gdiplus::Graphics g(hnd);
  if (!g_bitmap)
  {
    g_bitmap = std::make_unique<Gdiplus::Bitmap>(image.width, image.height, &g);
  }
  DrawImage(*g_bitmap, image);
  g.DrawImage(g_bitmap.get(), 0, 0);
}


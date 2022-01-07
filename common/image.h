#ifndef IMAGE_H
#define IMAGE_H

#include <algorithm>
#include <fstream>
#include <ostream>
#include <string>
#include <vector>

#include "geometry.h"

inline constexpr int32_t Colour(const Vec3f &colour, bool bgr_order = true)
{
  int32_t r = static_cast<std::uint8_t>(std::min(1.f, colour.x) * 255);
  int32_t g = static_cast<std::uint8_t>(std::min(1.f, colour.y) * 255);
  int32_t b = static_cast<std::uint8_t>(std::min(1.f, colour.z) * 255);

  if (bgr_order)
    return (b << 16) | (g << 8) | r;
  else
    return (r << 16) | (g << 8) | b;
}

inline constexpr int32_t Colour(float r_, float g_, float b_, bool bgr_order = true)
{
  int32_t r = static_cast<std::uint8_t>(std::min(1.f, r_) * 255);
  int32_t g = static_cast<std::uint8_t>(std::min(1.f, g_) * 255);
  int32_t b = static_cast<std::uint8_t>(std::min(1.f, b_) * 255);

  if (bgr_order)
    return (b << 16) | (g << 8) | r;
  else
    return (r << 16) | (g << 8) | b;
}

enum class ImageColour
{
  red   = Colour(1.f, 0.f, 0.f, false),
  green = Colour(0.f, 1.f, 0.f, false),
  blue  = Colour(0.f, 0.f, 1.f, false),
};

inline constexpr Vec3f Colour(ImageColour c)
{
  return {((static_cast<int32_t>(c) >> 16) & 0xff) / 255.f, ((static_cast<int32_t>(c) >> 8) & 0xff) / 255.f,
          ((static_cast<int32_t>(c) >> 0) & 0xff) / 255.f};
}

class Image
{
public:
  const int width;
  const int height;

  using Colour = Vec3f;

  Image(int width_, int height_)
      : width{width_}
      , height{height_}
      , pixels(width * static_cast<int64_t>(height))
  {
  }

  Colour Pixel(int x, int y) const { return pixels[y * static_cast<int64_t>(width) + x]; }
  void Pixel(int x, int y, const Colour &colour) { pixels[y * static_cast<int64_t>(width) + x] = colour; }
  void Pixel(int x, int y, const ImageColour &colour)
  {
    pixels[y * static_cast<int64_t>(width) + x] = ::Colour(colour);
  }

  const Vec3f &operator[](int idx) const { return pixels[idx]; }
  Vec3f &operator[](int idx) { return pixels[idx]; }

  auto begin() { return std::begin(pixels); }
  const auto begin() const { return std::begin(pixels); }
  auto end() { return std::end(pixels); }
  const auto end() const { return std::end(pixels); }

  friend void SaveToFile(std::string filename, const Image &i)
  {
    // Save result to a PPM image (keep these flags if you compile under
    // Windows)
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    ofs << "P6\n" << i.width << " " << i.height << "\n255\n";

    for (const auto &pxl : i.pixels)
    {
      ofs << static_cast<std::uint8_t>(std::min(1.f, pxl.x) * 255)
          << static_cast<std::uint8_t>(std::min(1.f, pxl.y) * 255)
          << static_cast<std::uint8_t>(std::min(1.f, pxl.z) * 255);
    }

    ofs.close();
  }

private:
  std::vector<Colour> pixels;
};

#endif // !IMAGE_H

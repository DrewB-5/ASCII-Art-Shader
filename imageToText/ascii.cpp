#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <windows.h>
#include <algorithm>
#include <cmath>

float gaussianKernel[5][5] = {
    {2, 4, 5, 4, 2},
    {4, 9, 12, 9, 4},
    {5, 12, 15, 12, 5},
    {4, 9, 12, 9, 4},
    {2, 4, 5, 4, 2}};
float gaussianWeight = 200.0f;

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cerr << "Usage: ascii_image <image_file>\n";
    return 1;
  }

  // required to use ■
  SetConsoleOutputCP(CP_UTF8);

  int width, height, channels;
  const char *filename = argv[1];
  unsigned char *img = stbi_load(filename, &width, &height, &channels, 1);
  if (!img)
  {
    std::cerr << "Failed to load image\n";
    return 1;
  }

  const std::vector<std::string> ascii = {u8" ", u8".", u8":", u8"c", u8"o", u8"P", u8"0", u8"?", u8"@", u8"■"};

  // target resolution
  const int outW = 292;
  const int outH = 82;
  // image pixels -> ASCII cells
  float blockW = static_cast<float>(width) / outW;
  float blockH = static_cast<float>(height) / outH;

  std::vector<std::vector<std::string>> asciiFrame(outH, std::vector<std::string>(outW));

  // applies gaussian blur
  std::vector<std::vector<float>> blurImg(height, std::vector<float>(width, 0.0f));
  for (int y = 2; y < height - 2; y++)
    for (int x = 2; x < width - 2; x++)
    {
      float sum = 0.0f;
      for (int ky = -2; ky <= 2; ky++)
        for (int kx = -2; kx <= 2; kx++)
          sum += img[(y + ky) * width + (x + kx)] * gaussianKernel[ky + 2][kx + 2];
      blurImg[y][x] = sum / gaussianWeight;
    }

  // sobel gradients (edge detection)
  int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
  int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

  std::vector<std::vector<float>> gradMag(height, std::vector<float>(width, 0.0f));
  std::vector<std::vector<float>> gradAngle(height, std::vector<float>(width, 0.0f));

  for (int y = 1; y < height - 1; y++)
    for (int x = 1; x < width - 1; x++)
    {
      float gx = 0, gy = 0;
      for (int ky = -1; ky <= 1; ky++)
        for (int kx = -1; kx <= 1; kx++)
        {
          float pixel = blurImg[y + ky][x + kx];
          gx += pixel * Gx[ky + 1][kx + 1];
          gy += pixel * Gy[ky + 1][kx + 1];
        }
      gradMag[y][x] = std::sqrt(gx * gx + gy * gy);
      float angle = std::atan2(gy, gx) * 180.0f / 3.14159265f;
      if (angle < 0)
        angle += 180.0f;
      gradAngle[y][x] = angle;
    }

  // edge thinning
  std::vector<std::vector<float>> nms(height, std::vector<float>(width, 0.0f));
  for (int y = 1; y < height - 1; y++)
    for (int x = 1; x < width - 1; x++)
    {
      float angle = gradAngle[y][x];
      float mag = gradMag[y][x];
      float q = 0.0f, r = 0.0f;

      if ((angle >= 0 && angle < 22.5f) || (angle >= 157.5f && angle <= 180))
      {
        q = gradMag[y][x + 1];
        r = gradMag[y][x - 1];
      }
      else if (angle >= 22.5f && angle < 67.5f)
      {
        q = gradMag[y + 1][x - 1];
        r = gradMag[y - 1][x + 1];
      }
      else if (angle >= 67.5f && angle < 112.5f)
      {
        q = gradMag[y + 1][x];
        r = gradMag[y - 1][x];
      }
      else if (angle >= 112.5f && angle < 157.5f)
      {
        q = gradMag[y - 1][x - 1];
        r = gradMag[y + 1][x + 1];
      }

      nms[y][x] = (mag >= q && mag >= r) ? mag : 0.0f;
    }

  // delete weak edges
  float tHigh = 100.0f, tLow = 30.0f;
  std::vector<std::vector<bool>> edges(height, std::vector<bool>(width, false));
  for (int y = 1; y < height - 1; y++)
    for (int x = 1; x < width - 1; x++)
    {
      if (nms[y][x] >= tHigh)
        edges[y][x] = true;
      else if (nms[y][x] >= tLow)
      {
        for (int ky = -1; ky <= 1; ky++)
          for (int kx = -1; kx <= 1; kx++)
            if (edges[y + ky][x + kx])
            {
              edges[y][x] = true;
              goto nextPixel;
            }
      }
    nextPixel:;
    }

  // -use angle to define edge type
  std::vector<std::vector<char>> edgeChar(height, std::vector<char>(width, ' '));
  for (int y = 1; y < height - 1; y++)
    for (int x = 1; x < width - 1; x++)
    {
      if (!edges[y][x])
        continue;
      float angle = gradAngle[y][x];
      char e = '_';
      if (angle < 22.5f || angle >= 157.5f)
        e = '|';
      else if (angle >= 22.5f && angle < 67.5f)
        e = '/';
      else if (angle >= 67.5f && angle < 112.5f)
        e = '-';
      else if (angle >= 112.5f && angle < 157.5f)
        e = '\\';
      edgeChar[y][x] = e;
    }

  // gamma correction
  unsigned char minVal = 255, maxVal = 0;
  for (int i = 0; i < width * height; i++)
  {
    minVal = std::min(minVal, img[i]);
    maxVal = std::max(maxVal, img[i]);
  }

  float gamma = .9f; // change gamma
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      if (!edges[y][x])
      {
        float normalized = (img[y * width + x] - minVal) / float(maxVal - minVal);
        normalized = std::pow(normalized, 1.0f / gamma);
        img[y * width + x] = static_cast<unsigned char>(std::min(255.0f, normalized * 255.0f));
      }
    }
  }

  // ASCII conversion
  std::vector<int> xRanges(outW + 1), yRanges(outH + 1);
  for (int i = 0; i <= outW; i++)
    xRanges[i] = std::min((int)(i * blockW), width);
  for (int i = 0; i <= outH; i++)
    yRanges[i] = std::min((int)(i * blockH), height);

  for (int y = 0; y < outH; y++)
  {
    int y0 = yRanges[y], y1 = yRanges[y + 1];
    for (int x = 0; x < outW; x++)
    {
      int x0 = xRanges[x], x1 = xRanges[x + 1];
      char e = ' ';
      int brightnessSum = 0, count = 0;

      for (int py = y0; py < y1; py++)
      {
        for (int px = x0; px < x1; px++)
        {
          if (edges[py][px])
            e = edgeChar[py][px];
          brightnessSum += img[py * width + px];
          count++;
        }
      }

      if (e != ' ')
        asciiFrame[y][x] = std::string(1, e);
      else
      {
        float avg = count > 0 ? (brightnessSum / (float)count) : 0.0f;
        int asciiIdx = (int)((avg / 255.0f) * (ascii.size() - 1));
        asciiFrame[y][x] = ascii[asciiIdx];
      }
    }
  }

  // binary output
  // reinterpret cast keeps bits the same just treats as a different type
  uint32_t w = outW, h = outH;
  std::cout.write(reinterpret_cast<const char *>(&w), sizeof(uint32_t));
  std::cout.write(reinterpret_cast<const char *>(&h), sizeof(uint32_t));
  for (const auto &row : asciiFrame)
    for (const auto &ch : row)
    {
      uint8_t len = static_cast<uint8_t>(ch.size());
      std::cout.write(reinterpret_cast<const char *>(&len), 1);
      std::cout.write(ch.data(), len);
    }
  std::cout.flush();

  stbi_image_free(img);
  return 0;
}

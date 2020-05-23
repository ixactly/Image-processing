#pragma once
#include <string>
#include <vector>

inline constexpr int HEADERSIZE     = 40;
inline constexpr int INFOHEADERSIZE = 14;

struct BitmapHeader {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    unsigned int biXPixPerMeter;
    unsigned int biYPixPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImporant;
} __attribute__((packed));

struct Image {
    Image() {}
    Image(int w, int h) : width(w), height(h), pixel(3 * w * h) {}

    Image(const Image &src)
    {
        this->width  = src.width;
        this->height = src.height;
        this->pixel  = src.pixel;
    }
    // height:j, width:iとして(x, y) = (i, j)と表記
    unsigned char &r(int i, int j)
    {
        return this->pixel.at(3 * (j * this->width + i) + 2);
    }
    unsigned char &g(int i, int j)
    {
        return this->pixel.at(3 * (j * this->width + i) + 1);
    }
    unsigned char &b(int i, int j)
    {
        return this->pixel.at(3 * (j * this->width + i));
    }

    unsigned char r_value(int i, int j) const
    {
        return this->pixel.at(3 * (j * this->width + i) + 2);
    }
    unsigned char g_value(int i, int j) const
    {
        return this->pixel.at(3 * (j * this->width + i) + 1);
    }
    unsigned char b_value(int i, int j) const
    {
        return this->pixel.at(3 * (j * this->width + i));
    }

    void reset(const int w, const int h)
    {
        this->width  = w;
        this->height = h;
        this->pixel.resize(3 * w * h);
    }

    int width;
    int height;
    std::vector<unsigned char> pixel;
};

void LoadBitmap(const std::string &path, Image *dst);

void SaveBitmap(const std::string &path, const Image &src);
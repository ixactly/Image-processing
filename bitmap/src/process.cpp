#include <cmath>
#include <functional>
#include <iobitmap.hpp>
#include <iostream>
#include <process.hpp>

auto bilinear = [](const double x, const double y, const Image &img, const char col) {
    const int x_floor = std::floor(x);
    const int y_floor = std::floor(y);

    if (col == 'r')
        return (x_floor + 1 - x) * (y_floor + 1 - y) * img.r_value(x_floor, y_floor) + (x_floor + 1 - x) * (y - y_floor) * img.r_value(x_floor, y_floor + 1) + (x - x_floor) * (y_floor + 1 - y) * img.r_value(x_floor + 1, y_floor) + (x - x_floor) * (y - y_floor) * img.r_value(x_floor + 1, y_floor + 1);
    else if (col == 'g')
        return (x_floor + 1 - x) * (y_floor + 1 - y) * img.g_value(x_floor, y_floor) + (x_floor + 1 - x) * (y - y_floor) * img.g_value(x_floor, y_floor + 1) + (x - x_floor) * (y_floor + 1 - y) * img.g_value(x_floor + 1, y_floor) + (x - x_floor) * (y - y_floor) * img.g_value(x_floor + 1, y_floor + 1);
    else if (col == 'b')
        return (x_floor + 1 - x) * (y_floor + 1 - y) * img.b_value(x_floor, y_floor) + (x_floor + 1 - x) * (y - y_floor) * img.b_value(x_floor, y_floor + 1) + (x - x_floor) * (y_floor + 1 - y) * img.b_value(x_floor + 1, y_floor) + (x - x_floor) * (y - y_floor) * img.b_value(x_floor + 1, y_floor + 1);
    else
        return 0.0;
};

// バイリニア変換により画像の容量を小さくする
void Reduction(Image *dst, Image &src, const int w, const int h)
{
    dst->reset(w, h);
    for (int i = 0; i < dst->width; ++i) {
        double x = src.width * i / dst->width;
        for (int j = 0; j < dst->height; ++j) {
            double y = src.height * j / dst->height;

            dst->r(i, j) = bilinear(x, y, src, 'r');
            dst->g(i, j) = bilinear(x, y, src, 'g');
            dst->b(i, j) = bilinear(x, y, src, 'b');
        }
    }
}

void Solarisation(Image *dst, const Image &src)
{
    const int w = dst->width;
    const int h = dst->height;

    //sinカーブを描くように調整を行う
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            dst->r(i, j) = 122 - std::round(122 * std::cos((src.r_value(i, j) / 256.0) * (5 * M_PI / 2)));
            dst->g(i, j) = 122 - std::round(122 * std::cos((src.g_value(i, j) / 256.0) * (5 * M_PI / 2)));
            dst->b(i, j) = 122 - std::round(122 * std::cos((src.b_value(i, j) / 256.0) * (5 * M_PI / 2)));
        }
    }
}

// bilinear
void Extend(Image *dst, const Image &src, double ratio)
{
    int x0, y0;
    // 切り抜きたい画像の左隅を選ぶ
    std::cout << "choose lower left point of image which you want to cut out." << std::endl;
    std::cout << "x(max: " << src.width / ratio << "): ";
    std::cin >> x0;
    std::cout << "y(max: " << src.height / ratio << "): ";
    std::cin >> y0;

    for (int i = 0; i < dst->width; ++i) {
        double x = x0 + (i / ratio);
        for (int j = 0; j < dst->height; ++j) {
            double y = y0 + (j / ratio);

            dst->r(i, j) = bilinear(x, y, src, 'r');
            dst->g(i, j) = bilinear(x, y, src, 'g');
            dst->b(i, j) = bilinear(x, y, src, 'b');
        }
    }
}

void VInverse(Image *dst, const Image &src)
{
    for (int i = 0; i < dst->width; ++i) {
        const int x = std::abs(i - dst->width / 2) + dst->width / 2 - 1;
        for (int j = 0; j < dst->height; ++j) {
            dst->r(i, j) = src.r_value(x, j);
            dst->g(i, j) = src.g_value(x, j);
            dst->b(i, j) = src.b_value(x, j);
        }
    }
}

void Tunnel(Image *dst, const Image &src)
{
    int x0, y0, r;
    // 効果を持たせる範囲を定める
    std::cout << "Picture Info    width: " << dst->width << " height: " << dst->height << std::endl;
    std::cout << "Set radius." << std::endl;
    std::cout << "r: ";
    std::cin >> r;
    std::cout << "Set the center of circle." << std::endl;
    std::cout << "x: ";
    std::cin >> x0;
    std::cout << "y: ";
    std::cin >> y0;

    for (int i = 0; i < dst->width; ++i) {
        for (int j = 0; j < dst->height; ++j) {
            if (std::sqrt((i - x0) * (i - x0) + (j - y0) * (j - y0)) > r) {
                double theta = std::atan2(j - y0, i - x0);
                dst->r(i, j) = src.r_value(x0 + std::round(r * std::cos(theta)), y0 + std::round(r * std::sin(theta)));
                dst->g(i, j) = src.g_value(x0 + std::round(r * std::cos(theta)), y0 + std::round(r * std::sin(theta)));
                dst->b(i, j) = src.b_value(x0 + std::round(r * std::cos(theta)), y0 + std::round(r * std::sin(theta)));

            } else {
                dst->r(i, j) = src.r_value(i, j);
                dst->g(i, j) = src.g_value(i, j);
                dst->b(i, j) = src.b_value(i, j);
            }
        }
    }
}
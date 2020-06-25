#include <cmath>
#include <functional>
#include <iobitmap.hpp>
#include <iostream>
#include <process.hpp>
#include <vector>

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

void GrayScale(Image *dst, const Image &src)
{
    for (int i = 0; i < src.width; ++i) {
        for (int j = 0; j < src.height; ++j) {
            dst->r(i, j) = (src.r_value(i, j) + src.g_value(i, j) + src.b_value(i, j)) / 3.0;
            dst->g(i, j) = (src.r_value(i, j) + src.g_value(i, j) + src.b_value(i, j)) / 3.0;
            dst->b(i, j) = (src.r_value(i, j) + src.g_value(i, j) + src.b_value(i, j)) / 3.0;
        }
    }
}
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

void Bilateral_Filt(Image *dst, const Image &src)
{
    // (i, j) = (x, y)
    int w = src.width;
    int h = src.height;

    // zero padding
    Image tmp(w + 4, h + 4);

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            tmp.r(i + 2, j + 2) = src.r_value(i, j);
            tmp.g(i + 2, j + 2) = src.g_value(i, j);
            tmp.b(i + 2, j + 2) = src.b_value(i, j);
        }
    }

    std::array<int, 5> ar = {1, 4, 6, 4, 1};
    std::array<std::array<double, 5>, 5> gaussian;
    double sum = 0, sum_j = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            sum += ar[i] * ar[j];
            gaussian[i][j] = ar[i] * ar[j] / 256.0;
            sum_j += gaussian[i][j];
        }
    }

    double mean = 0, var = 0;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            mean += src.r_value(i, j) + src.g_value(i, j) + src.b_value(i, j);
        }
    }

    mean /= 3 * w * h;

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            var += std::pow(mean - src.r_value(i, j), 2) + std::pow(mean - src.g_value(i, j), 2) + std::pow(mean - src.b_value(i, j), 2);
        }
    }
    var /= 3 * w * h;
    std::cout << "mean: " << mean << std::endl;
    std::cout << "var: " << var << std::endl;

    //filtering
    std::array<std::array<double, 5>, 5> b_filt_r, b_filt_g, b_filt_b;
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            for (int m = 0; m < 5; ++m) {
                for (int n = 0; n < 5; ++n) {
                    b_filt_r[m][n] = gaussian[m][n] * std::exp(-std::pow(tmp.r_value(i + 2, j + 2) - tmp.r_value(i + m, j + n), 2) / (2 * var));
                    b_filt_g[m][n] = gaussian[m][n] * std::exp(-std::pow(tmp.g_value(i + 2, j + 2) - tmp.g_value(i + m, j + n), 2) / (2 * var));
                    b_filt_b[m][n] = gaussian[m][n] * std::exp(-std::pow(tmp.b_value(i + 2, j + 2) - tmp.b_value(i + m, j + n), 2) / (2 * var));
                }
            }

            double sum_r1 = 0, sum_g1 = 0, sum_b1 = 0;
            double sum_r2 = 0, sum_g2 = 0, sum_b2 = 0;

            for (int m = 0; m < 5; ++m) {
                for (int n = 0; n < 5; ++n) {
                    sum_r1 += b_filt_r[m][n];
                    sum_r2 += b_filt_r[m][n] * tmp.r_value(i + m, j + n);

                    sum_g1 += b_filt_g[m][n];
                    sum_g2 += b_filt_g[m][n] * tmp.g_value(i + m, j + n);

                    sum_b1 += b_filt_b[m][n];
                    sum_b2 += b_filt_b[m][n] * tmp.b_value(i + m, j + n);
                }
            }

            dst->r(i, j) = sum_r2 / sum_r1;
            dst->g(i, j) = sum_g2 / sum_g1;
            dst->b(i, j) = sum_b2 / sum_b1;
        }
    }
}

void Gauss_Filt(Image *dst, const Image &src)
{

    int w = src.width;
    int h = src.height;

    // zero padding
    Image tmp(w + 4, h + 4);

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            tmp.r(i + 2, j + 2) = src.r_value(i, j);
            tmp.g(i + 2, j + 2) = src.g_value(i, j);
            tmp.b(i + 2, j + 2) = src.b_value(i, j);
        }
    }

    std::array<int, 5> ar = {1, 4, 6, 4, 1};
    std::array<std::array<double, 5>, 5> gaussian;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            gaussian[i][j] = ar[i] * ar[j] / 256.0;
        }
    }

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            double sum_r = 0, sum_g = 0, sum_b = 0;
            for (int m = 0; m < 5; ++m) {
                for (int n = 0; n < 5; ++n) {
                    sum_r += gaussian[m][n] * tmp.r_value(i + m, j + n);
                    sum_g += gaussian[m][n] * tmp.g_value(i + m, j + n);
                    sum_b += gaussian[m][n] * tmp.b_value(i + m, j + n);
                }
            }
            dst->r(i, j) = sum_r;
            dst->g(i, j) = sum_g;
            dst->b(i, j) = sum_b;
        }
    }
}

void Gradient(Image *dst, const Image &src)
{
    int sobel_filt_x[3][3] = {{-1, -2, -1},
                              {0, 0, 0},
                              {1, 2, 1}};

    int soble_filt_y[3][3] = {{-1, 0, 1},
                              {-2, 0, 2},
                              {-1, 0, 1}};

    int w = src.width;
    int h = src.height;

    std::vector<std::vector<double>> grad_r(w - 2, std::vector<double>(h - 2, 0));
    std::vector<std::vector<double>> grad_g(w - 2, std::vector<double>(h - 2, 0));
    std::vector<std::vector<double>> grad_b(w - 2, std::vector<double>(h - 2, 0));
    std::vector<std::vector<double>> mean(w - 2, std::vector<double>(h - 2, 0));

    double max_mean = 0;

    for (int i = 0; i < w - 2; ++i) {
        for (int j = 0; j < h - 2; ++j) {
            std::array<int, 3> grad_x = {0, 0, 0};
            std::array<int, 3> grad_y = {0, 0, 0};

            for (int m = 0; m < 3; ++m) {
                for (int n = 0; n < 3; ++n) {
                    grad_x[0] += sobel_filt_x[m][n] * src.r_value(i + m, j + n);
                    grad_y[0] += soble_filt_y[m][n] * src.r_value(i + m, j + n);
                }
            }
            grad_r[i][j] = std::sqrt(grad_x[0] * grad_x[0] + grad_y[0] * grad_y[0]);

            for (int m = 0; m < 3; ++m) {
                for (int n = 0; n < 3; ++n) {
                    grad_x[1] += sobel_filt_x[m][n] * src.g_value(i + m, j + n);
                    grad_y[1] += soble_filt_y[m][n] * src.g_value(i + m, j + n);
                }
            }
            grad_g[i][j] = std::sqrt(grad_x[1] * grad_x[1] + grad_y[1] * grad_y[1]);

            for (int m = 0; m < 3; ++m) {
                for (int n = 0; n < 3; ++n) {
                    grad_x[2] += sobel_filt_x[m][n] * src.b_value(i + m, j + n);
                    grad_y[2] += soble_filt_y[m][n] * src.b_value(i + m, j + n);
                }
            }
            grad_b[i][j] = std::sqrt(grad_x[2] * grad_x[2] + grad_y[2] * grad_y[2]);

            mean[i][j] = (grad_r[i][j] + grad_g[i][j] + grad_b[i][j]) / 3.0;
            if (mean[i][j] > max_mean) {
                max_mean = mean[i][j];
            }
        }
    }
    std::cout << max_mean << std::endl;
    dst->reset(w - 2, h - 2);
    for (int i = 0; i < w - 2; ++i) {
        for (int j = 0; j < h - 2; ++j) {
            dst->r(i, j) = mean[i][j] * 255 / max_mean;
            dst->g(i, j) = mean[i][j] * 255 / max_mean;
            dst->b(i, j) = mean[i][j] * 255 / max_mean;
        }
    }
}

void BinaryImage(Image *dst, const Image &src, const int threshold)
{
    GrayScale(dst, src);
    for (int i = 0; i < src.width; i++) {
        for (int j = 0; j < src.height; j++) {
            if (dst->r_value(i, j) < threshold) {
                dst->r(i, j) = 0;
                dst->g(i, j) = 0;
                dst->b(i, j) = 0;
            } else {
                dst->r(i, j) = 255;
                dst->g(i, j) = 255;
                dst->b(i, j) = 255;
            }
        }
    }
}

//Hit & Fit
void Expansion(Image *dst, const Image &src)
{
    const int w = src.width;
    const int h = src.height;

    // zero padding
    Image tmp(w + 2, h + 2);

    for (int i = 0; i < w + 2; ++i) {
        for (int j = 0; j < h + 2; ++j) {
            if (i == 0 || i == (w + 1) || j == 0 || j == (h + 1))
                tmp.r(i, j) = 255;
            else
                tmp.r(i, j) = src.r_value(i - 1, j - 1);
        }
    }

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            bool flag = false;
            for (int m = 0; m < 3; ++m) {
                if (flag)
                    break;
                for (int n = 0; n < 3; ++n) {
                    if (tmp.r_value(i + m, j + n) == 0) {
                        flag = true;
                        break;
                    }
                }
            }
            if (flag) {
                dst->r(i, j) = 0;
                dst->g(i, j) = 0;
                dst->b(i, j) = 0;
            }
        }
    }
}

void Shrink(Image *dst, const Image &src)
{
    const int w = src.width;
    const int h = src.height;

    // zero padding
    Image tmp(w + 2, h + 2);

    for (int i = 0; i < w + 2; ++i) {
        for (int j = 0; j < h + 2; ++j) {
            if (i == 0 || i == (w + 1) || j == 0 || j == (h + 1))
                tmp.r(i, j) = 0;
            else
                tmp.r(i, j) = src.r_value(i - 1, j - 1);
        }
    }

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            bool flag = false;
            for (int m = 0; m < 3; ++m) {
                if (flag)
                    break;
                for (int n = 0; n < 3; ++n) {
                    if (tmp.r_value(i + m, j + n) != 0) {
                        flag = true;
                        break;
                    }
                }
            }
            if (flag) {
                dst->r(i, j) = 255;
                dst->g(i, j) = 255;
                dst->b(i, j) = 255;
            }
        }
    }
}

void Centroid(Image *dst, const Image &src)
{
    int count  = 0;
    double x_g = 0, y_g = 0;
    for (int i = 0; i < src.width; ++i) {
        for (int j = 0; j < src.height; ++j) {
            if (src.r_value(i, j) == 0) {
                x_g += i;
                y_g += j;
                count++;
            }
        }
    }

    for (int i = 0; i < 5; i++) {
        dst->r(static_cast<int>(x_g / count) - 2 + i, static_cast<int>(y_g / count)) = 255;
        dst->g(static_cast<int>(x_g / count) - 2 + i, static_cast<int>(y_g / count)) = 0;
        dst->b(static_cast<int>(x_g / count) - 2 + i, static_cast<int>(y_g / count)) = 0;
    }

    for (int j = 0; j < 5; j++) {
        dst->r(static_cast<int>(x_g / count), static_cast<int>(y_g / count) - 2 + j) = 255;
        dst->g(static_cast<int>(x_g / count), static_cast<int>(y_g / count) - 2 + j) = 0;
        dst->b(static_cast<int>(x_g / count), static_cast<int>(y_g / count) - 2 + j) = 0;
    }
}

void Frame(Image *dst, const Image &src)
{
    int w_min = src.width, w_max = 0;
    int h_min = src.height, h_max = 0;

    for (int i = 0; i < src.width; i++) {
        for (int j = 0; j < src.height; j++) {
            if (src.r_value(i, j) == 0) {
                if (i < w_min)
                    w_min = i;
                if (i > w_max)
                    w_max = i;
                if (j < h_min)
                    h_min = j;
                if (j > h_max)
                    h_max = j;
            }
        }
    }

    std::cout << w_min << " " << w_max << " " << h_min << " " << h_max;
    for (int i = w_min - 1; i <= w_max + 1; i++) {
        for (int j = h_min - 1; j <= h_max + 1; j++) {
            if ((i == (w_min - 1)) || (i == (w_max + 1)) || (j == (h_min - 1)) || (j == (h_max + 1))) {
                dst->g(i, j) = 200;
                dst->r(i, j) = 200;
                dst->b(i, j) = 0;
            }
        }
    }
}

void NCC(Image *dst, const Image &src, const Image &tmp)
{
    const int w = src.width - tmp.width;
    const int h = src.height - tmp.height;
    dst->reset(w, h);

    // ncc値を格納する配列
    std::vector<double> R_list(w * h);
    std::array<std::tuple<double, int, int>, 4> R_top;

    // nccの最大と最小を求め，[0, 255]でスケーリング
    double R_min = 1, R_max = 0;

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            double numer = 0, denom1 = 0, denom2 = 0;
            for (int m = 0; m < tmp.width; m++) {
                for (int n = 0; n < tmp.height; n++) {
                    numer += src.r_value(i + m, j + n) * tmp.r_value(m, n);
                    denom1 += std::pow(src.r_value(i + m, j + n), 2);
                    denom2 += std::pow(tmp.r_value(m, n), 2);
                }
            }

            double R_ncc = numer / std::sqrt(denom1 * denom2);
            if (R_min > R_ncc)
                R_min = R_ncc;
            if (R_max < R_ncc)
                R_max = R_ncc;

            R_list[j * w + i] = R_ncc;
            R_top[3]          = std::make_tuple(R_ncc, i, j);
            // std::cout << std::get<0>(R_top[3]) << " " << std::get<1>(R_top[3]) << " " << std::get<2>(R_top[3]) << std::endl;

            std::sort(R_top.begin(), R_top.end(), [](const auto &lhs, const auto &rhs) {
                return std::get<0>(lhs) > std::get<0>(rhs);
            });

            // 同一の領域におけるセットを避け

            if (std::sqrt(std::pow(std::get<1>(R_top[0]) - std::get<1>(R_top[1]), 2) + std::pow(std::get<2>(R_top[0]) - std::get<2>(R_top[1]), 2)) < 5) {
                R_top[1] = std::make_tuple(0.0, 0, 0);
            }
        }
    }

    // scaling
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            dst->r(i, j) = static_cast<int>((R_list[j * w + i] - R_min) * 255 / (R_max - R_min));
            dst->g(i, j) = static_cast<int>((R_list[j * w + i] - R_min) * 255 / (R_max - R_min));
            dst->b(i, j) = static_cast<int>((R_list[j * w + i] - R_min) * 255 / (R_max - R_min));
        }
    }
    for (int i = 0; i < 3; i++) {
        dst->r(std::get<1>(R_top[i]), std::get<2>(R_top[i])) = 0;
        dst->g(std::get<1>(R_top[i]), std::get<2>(R_top[i])) = 0;
        dst->b(std::get<1>(R_top[i]), std::get<2>(R_top[i])) = 0;
    }
    dst->r(std::get<1>(R_top[0]), std::get<2>(R_top[0])) = 255;
    dst->g(std::get<1>(R_top[1]), std::get<2>(R_top[1])) = 255;
    dst->b(std::get<1>(R_top[2]), std::get<2>(R_top[2])) = 255;
}

void NCCFrame(Image *dst, const Image &src, const Image &tmp)
{
    const int w = src.width - tmp.width;
    const int h = src.height - tmp.height;

    // ncc値を格納する配列
    std::vector<double> R_list(w * h);
    std::array<std::tuple<double, int, int>, 4> R_top;

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            double numer = 0, denom1 = 0, denom2 = 0;
            for (int m = 0; m < tmp.width; m++) {
                for (int n = 0; n < tmp.height; n++) {
                    numer += src.r_value(i + m, j + n) * tmp.r_value(m, n);
                    denom1 += std::pow(src.r_value(i + m, j + n), 2);
                    denom2 += std::pow(tmp.r_value(m, n), 2);
                }
            }
            double R_ncc = numer / std::sqrt(denom1 * denom2);

            // ncc値をtop3と新しいncc値1つの計4つでsortしていく
            R_top[3] = std::make_tuple(R_ncc, i, j);
            std::sort(R_top.begin(), R_top.end(), [](const auto &lhs, const auto &rhs) {
                return std::get<0>(lhs) > std::get<0>(rhs);
            });

            // 近い領域におけるncc値のtopを避ける
            if (std::sqrt(std::pow(std::get<1>(R_top[0]) - std::get<1>(R_top[1]), 2) + std::pow(std::get<2>(R_top[0]) - std::get<2>(R_top[1]), 2)) < 5) {
                R_top[1] = std::make_tuple(0.0, 0, 0);
            }
        }
    }

    const int &i1 = std::get<1>(R_top[0]), &j1 = std::get<2>(R_top[0]);
    // top1 ncc
    for (int i = 0; i < tmp.width; i++) {
        dst->low(i1 + i, j1);
        dst->r(i1 + i, j1) = 255;
        dst->low(i1 + i, j1 + tmp.height - 1);
        dst->r(i1 + i, j1 + tmp.height - 1) = 255;
    }

    for (int j = 0; j < tmp.height; j++) {
        dst->low(i1, j1 + j);
        dst->r(i1, j1 + j) = 255;
        dst->low(i1 + tmp.width - 1, j1 + j);
        dst->r(i1 + tmp.width - 1, j1 + j) = 255;
    }

    // top2 ncc
    const int &i2 = std::get<1>(R_top[1]), &j2 = std::get<2>(R_top[1]);
    for (int i = 0; i < tmp.width; i++) {
        dst->low(i2 + i, j2);
        dst->g(i2 + i, j2) = 255;
        dst->low(i2 + i, j2 + tmp.height);
        dst->g(i2 + i, j2 + tmp.height) = 255;
    }
    for (int j = 0; j < tmp.height; j++) {
        dst->low(i2, j2 + j);
        dst->g(i2, j2 + j) = 255;
        dst->low(i2 + tmp.width - 1, j2 + j);
        dst->g(i2 + tmp.width - 1, j2 + j) = 255;
    }

    const int &i3 = std::get<1>(R_top[2]), &j3 = std::get<2>(R_top[2]);
    for (int i = 0; i < tmp.width; i++) {
        dst->low(i3 + i, j3);
        dst->b(i3 + i, j3) = 255;
        dst->low(i3 + i, j3 + tmp.height);
        dst->b(i3 + i, j3 + tmp.height) = 255;
    }
    for (int j = 0; j < tmp.height; j++) {
        dst->low(i3, j3 + j);
        dst->b(i3, j3 + j) = 255;
        dst->low(i3 + tmp.width - 1, j3 + j);
        dst->b(i3 + tmp.width - 1, j3 + j) = 255;
    }
}
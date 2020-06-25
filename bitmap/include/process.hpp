#include "iobitmap.hpp"

void GrayScale(Image *dst, const Image &src);
void Reduction(Image *dst, Image &src, const int w, const int h);
void Solarisation(Image *dst, const Image &src);
void Extend(Image *dst, const Image &src, double ratio);
void VInverse(Image *dst, const Image &src);
void Tunnel(Image *dst, const Image &src);
void Bilateral_Filt(Image *dst, const Image &src);
void Gauss_Filt(Image *dst, const Image &src);
void Gradient(Image *dst, const Image &src);
void BinaryImage(Image *dst, const Image &src, const int threshold);
void Expansion(Image *dst, const Image &src);
void Shrink(Image *dst, const Image &src);
void Centroid(Image *dst, const Image &src);
void Frame(Image *dst, const Image &src);
void NCC(Image *dst, const Image &src, const Image &tmp);
void NCCFrame(Image *dst, const Image &src, const Image &tmp);
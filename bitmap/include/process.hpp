#include "iobitmap.hpp"

void Reduction(Image *dst, Image &src, const int w, const int h);
void Solarisation(Image *dst, const Image &src);
void Extend(Image *dst, const Image &src, double ratio);
void VInverse(Image *dst, const Image &src);
void Tunnel(Image *dst, const Image &src);
void Bilateral_Filt(Image *dst, const Image &src);
void Gauss_Filt(Image *dst, const Image &src);
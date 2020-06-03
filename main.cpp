#include <iobitmap.hpp>
#include <process.hpp>
#include <string>

int main()
{
    Image img;
    LoadBitmap(std::string("../image/03193006 森 智希 in.bmp"), &img);
    Image dst(img);
    // Solarisation(&dst, img);
    // Extend(&dst, img, 2);
    // Tunnel(&dst, img);
    Gauss_Filt(&dst, img);
    SaveBitmap(std::string("../image/sunset_out.bmp"), dst);
}
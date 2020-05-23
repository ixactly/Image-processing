#include <iobitmap.hpp>
#include <process.hpp>
#include <string>

int main()
{
    Image img;
    LoadBitmap(std::string("../image/03193006 森 智希 in.bmp"), &img);
    Image dst;
    // Solarisation(&dst, img);
    // Extend(&dst, img, 2);
    // Tunnel(&dst, img);
    Reduction(&dst, img, img.width / 6, img.height / 6);
    SaveBitmap(std::string("../image/03193006 森 智希 in.bmp"), dst);
}
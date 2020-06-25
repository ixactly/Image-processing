#include <iobitmap.hpp>
#include <process.hpp>
#include <string>

int main()
{
    Image src, tmp;
    LoadBitmap(std::string("../image/gopochi-all.bmp"), &src);
    Image dst{src};
    LoadBitmap(std::string("../image/gopochi-template.bmp"), &tmp);
    NCCFrame(&dst, src, tmp);
    SaveBitmap(std::string("../image/gochipo_ncc_frame.bmp"), dst);
}
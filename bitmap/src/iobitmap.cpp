#include <cstdlib>
#include <fstream>
#include <iobitmap.hpp>
#include <iostream>
#include <string>
#include <vector>

// 画像処理するための情報を抜き出す
void LoadBitmap(const std::string &path, Image *dst)
{
    std::ifstream ifs(path, std::ios::binary);
    if (ifs.fail()) {
        std::cout << "No such file. Exit." << std::endl;
        std::exit(0);
    }

    BitmapHeader header;
    // bmpのバイナリをバイト単位でそのままBitmapHeaderに読み込み
    ifs.read(reinterpret_cast<char *>(&header), sizeof(header));

    int w = std::abs(header.biWidth);
    int h = std::abs(header.biHeight);

    /*
    std::cout << "header:" << sizeof(header) << std::endl;
    std::cout << "width: " << w << " height: " << h << std::endl;
    */

    char buf = 0;
    dst->pixel.resize(3 * w * h);

    /*
    widthのバイト数は4の倍数で区切られているため0x00を列の最後に含むことがある
    空白の個数はwidth%4で表すことができる
    空白のバイトはbufで回避して読み込む．

    座標は画像左下を原点として(x, y) = (i, j)
    */

    /*
    std::cout << "ofbits: " << header.bfOffBits << std::endl;
    std::cout << "bitcount: " << header.biBitCount << std::endl;
    */

    for (int i = 0; i < h; i++) {
        ifs.read(reinterpret_cast<char *>(dst->pixel.data() + i * (3 * w + w % 4)), 3 * w);
        ifs.read(&buf, w % 4);
    }

    dst->width  = w;
    dst->height = h;

    ifs.close();
}

void SaveBitmap(const std::string &path, const Image &src)
{
    std::ofstream ofs(path, std::ios::binary);

    BitmapHeader header;
    header.bfType         = 0x4d42;
    header.bfSize         = (src.width + src.width % 4) * src.height + HEADERSIZE;
    header.bfReserved1    = 0;
    header.bfReserved2    = 0;
    header.bfOffBits      = HEADERSIZE + INFOHEADERSIZE;
    header.biSize         = HEADERSIZE;
    header.biWidth        = src.width;
    header.biHeight       = src.height;
    header.biPlanes       = 1;
    header.biBitCount     = 24;
    header.biCompression  = 0;
    header.biSizeImage    = 0;
    header.biXPixPerMeter = 0;
    header.biYPixPerMeter = 0;
    header.biClrUsed      = 0;
    header.biClrImporant  = 0;

    char buf = 0;
    ofs.write(reinterpret_cast<char *>(&header), sizeof(header));
    for (int i = 0; i < src.height; i++) {
        ofs.write(reinterpret_cast<const char *>(src.pixel.data() + i * 3 * src.width), 3 * src.width);
        ofs.write(&buf, src.width % 4);
    }

    ofs.close();
}
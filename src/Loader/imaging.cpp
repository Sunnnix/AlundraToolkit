#include "imaging.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define PaletteSize 64

/*  _____                                     _____  */
/* ( ___ )                                   ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  ____  _ _                          |   |  */
/*  |   | | __ )(_) |_ _ __ ___   __ _ _ __   |   |  */
/*  |   | |  _ \| | __| '_ ` _ \ / _` | '_ \  |   |  */
/*  |   | | |_) | | |_| | | | | | (_| | |_) | |   |  */
/*  |   | |____/|_|\__|_| |_| |_|\__,_| .__/  |   |  */
/*  |   |                             |_|     |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                                   (_____) */

Bitmap::Bitmap(int width, int height, char* pal)
{
    bool indexed = pal != NULL;
    int row = (width << (!indexed * 3)) >> 1;

    uint32_t offBits = sizeof(BitmapHeader)
                     + sizeof(BitmapInfo)
                     + PaletteSize * indexed;

    uint32_t fileSize = offBits + row * height;

    // Memory Allocation

    _base = (char*) malloc(fileSize);
    memset(_base, 0, fileSize);

    // Set Pointers

    _bmh = (BitmapHeader*) _base;
    _bmi = (BitmapInfo*) (_base + sizeof(BitmapHeader));
    _pal = NULL;
    _data = (char*) (_base + offBits);

    // Set BitmapHeader

    _bmh->Type = 0x4D42;
    _bmh->FileSize = fileSize;
    _bmh->Reserved1 = 0;
    _bmh->Reserved2 = 0;
    _bmh->OffBits = offBits;

    // Set BitmapInfo

    _bmi->InfoSize = sizeof(BitmapInfo);
    _bmi->Width = width;
    _bmi->Height = height;
    _bmi->Planes = 1;
    _bmi->BitCount = 4 << (!indexed * 3);   // Depth
    _bmi->Compression = 0;
    _bmi->ImageSize = 0;
    _bmi->XRes = 0;
    _bmi->YRes = 0;
    _bmi->UsedColors = 0;
    _bmi->NeededColors = 0;

    // Set Palette

    if (indexed)
    {
        _pal = (Color*) (_base + sizeof(BitmapHeader) + sizeof(BitmapInfo));
        memcpy(_pal, pal, PaletteSize);
    }
}

int Bitmap::SetPixel(uint32_t x, uint32_t y, Color clr)
{
    if (x > _bmi->Width || y > _bmi->Height)
    {
        printf("Warning: Out of bounds in Bitmap::SetPixel()\n");
        return 1;
    }

    // Copy the Color struct

    ((Color*) _data)[(_bmi->Height - y - 1) * _bmi->Width + x] = clr;

    return 0;
}

void Bitmap::SetBits(char* src)
{
    int row = ((_bmi->Width * _bmi->BitCount) >> 5) << 2;

    if (src != NULL)
    {
        for (uint32_t i = 0; i < _bmi->Height; i++)
        {
            memcpy(&((_data)[i * row]), &src[(_bmi->Height - i - 1) * row], row);
        }
    }
}

Bitmap::~Bitmap()
{
    free(_base);
}

/*  _____                                 _____  */
/* ( ___ )                               ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  ____       _      _   _        |   |  */
/*  |   | |  _ \ __ _| | ___| |_| |_ ___  |   |  */
/*  |   | | |_) / _` | |/ _ \ __| __/ _ \ |   |  */
/*  |   | |  __/ (_| | |  __/ |_| ||  __/ |   |  */
/*  |   | |_|   \__,_|_|\___|\__|\__\___| |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                               (_____) */

Palette::Palette()
{
    _palEntries = 0;
    _bmp = NULL;
}

int Palette::SetEntries(uint16_t* src, int palCount)
{
    _palEntries = palCount;
    uint16_t clr = 0;

    _bmp = new Bitmap(16, _palEntries, NULL);
    _data = (char*) malloc(_palEntries << 6);

    if (!_bmp || !_data)
    {
        fprintf(stderr, "Memory allocation failed: in Palette::SetEntries()\n");
        return 1;
    }

    for (int y = 0; y < _palEntries; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            clr = *(src + (y << 4) + x);
            ((Color*) _data)[(y << 4) + x] = Color { (char) ((clr & 0x7C00) >> 7),
                                                     (char) ((clr & 0x03E0) >> 2),
                                                     (char) ((clr & 0x001F) << 3),
                                                     (char) (clr >> 15) };
        }
    }

    _bmp->SetBits(_data);

    return 0;
}

Palette::~Palette()
{
    free(_data);
    delete _bmp;
}

/*  _____                                    _____  */
/* ( ___ )                                  ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  _____         _                   |   |  */
/*  |   | |_   _|____  _| |_ _   _ _ __ ___  |   |  */
/*  |   |   | |/ _ \ \/ / __| | | | '__/ _ \ |   |  */
/*  |   |   | |  __/>  <| |_| |_| | | |  __/ |   |  */
/*  |   |   |_|\___/_/\_\\__|\__,_|_|  \___| |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                                  (_____) */

Texture::Texture()
{
    _size = 0;
    _pal = NULL;
    _data = NULL;
    _bmp = NULL;
}

void Texture::SetTexture(char* texSrc, uint16_t* palSrc)
{
    _pal = new Palette();
    _pal->SetEntries(palSrc, 32);

    _size = Unzip(texSrc, &_data);

    _bmp = new Bitmap(256, _size >> 7, _pal->GetPalette(0));
    _bmp->SetBits(_data);
}

uint32_t Texture::Unzip(char* src, char** dst)
{
    uint32_t rhead = 6;
    uint32_t whead = 0;
    uint32_t seek = 0;
    uint8_t cur = 0;
    uint8_t dist = 0;
    uint8_t len = 0;
    bool keep = true;

    char* buffer = (char*) malloc(0x80000);

    while (keep && whead < 0x80000)
    {
        cur = src[rhead++];

        if (cur == 0xAD)
        {
            dist = src[rhead++];

            if (dist == 0)
            {
                buffer[whead++] = bswap(cur);
            }
            else
            {
                len = src[rhead++];
                keep = !(cur == 0xAD && dist == 0xFF && len == 0x00);
                seek = whead - dist;

                while (len-- > 0)
                {
                    buffer[whead++] = buffer[seek++];
                }
            }
        }
        else
        {
            buffer[whead++] = bswap(cur);
        }
    }

    *dst = (char*) realloc(buffer, whead);

    return whead;
}

Texture::~Texture()
{
    delete _pal;
    free(_data);
    delete _bmp;
}

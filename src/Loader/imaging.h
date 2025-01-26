#ifndef IMAGINGE_H_INCLUDED
#define IMAGINGE_H_INCLUDED

#include <cstdint>

struct Color
{
    char    Blue;
    char    Green;
    char    Red;
    char    Reserved;
};

struct BitmapHeader
{
    uint16_t    Type;       // Always 0x4D42
    uint32_t    FileSize;
    uint16_t    Reserved1;
    uint16_t    Reserved2;
    uint32_t    OffBits;    // Pointer to the bytes
}   __attribute__((packed, aligned(2)));

struct BitmapInfo
{
    uint32_t     InfoSize;
    uint32_t     Width;
    uint32_t     Height;
    uint16_t     Planes;
    uint16_t     BitCount;
    uint32_t     Compression;
    uint32_t     ImageSize;
    uint32_t     XRes;
    uint32_t     YRes;
    uint32_t     UsedColors;
    uint32_t     NeededColors;
};

class Bitmap
{
    private:

    char*           _base;

    BitmapHeader*   _bmh;
    BitmapInfo*     _bmi;
    Color*          _pal;
    char*           _data;

    public:

    /// Constructor
    Bitmap(int width, int height, char* pal);

    /// Set a single pixel at the said position
    int SetPixel(uint32_t x, uint32_t y, Color color);
    /// Fill Bitmap from a source
    void SetBits(char* src);

    // Getters

    char*       GetBasePtr()    { return _base; }
    uint32_t    GetFileSize()   { return _bmh->FileSize; }
    char*       GetBits()       { return _data; }

    /// Destructor
    ~Bitmap();
};

class Palette
{
    private:

    int         _palEntries;
    char*       _data;
    Bitmap*     _bmp;

    public:

    /// Constructor
    Palette();

    /// Set the palettes
    int SetEntries(uint16_t* src, int palCount);

    // Getters

    char*   GetPalette(int dex)   { return (char*) (_data + (dex << 6)); }

    /// Destructor
    ~Palette();
};

class Texture
{
    private:

    uint32_t    _size;
    Palette*    _pal;
    char*       _data;
    Bitmap*     _bmp;

    /// Swap uint8_t nibbles
    inline uint8_t bswap(uint8_t x) { return (uint8_t) ((x << 4) | (x >> 4)); }

    public:

    /// Constructor
    Texture();

    /// Unzip dictionnary encoded texture
    uint32_t Unzip(char* src, char** dst);
    /// Set the texture
    void SetTexture(char* texSrc, uint16_t* palSrc);

    /// Destructor
    ~Texture();
};

#endif // IMAGINGE_H_INCLUDED

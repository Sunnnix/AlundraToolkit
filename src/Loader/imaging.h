#ifndef IMAGINGE_H_INCLUDED
#define IMAGINGE_H_INCLUDED

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include "../Graphics/Shader.h"

struct Point
{
    int    X;
    int    Y;
};

struct Triangle
{
    Point   P0;
    Point   P1;
    Point   P2;
};

struct Rect
{
    int    X;
    int    Y;
    int    Width;
    int    Height;
};

struct Quad
{
    Point V0;
    Point V1;
    Point V2;
    Point V3;
};

struct Vertex
{
    float X, Y;
    float U, V;
    int Group;
    int PalDex;
    int BlendMode;
};

struct Color
{
    uint8_t    Red;
    uint8_t    Green;
    uint8_t    Blue;
    uint8_t    Alpha;

    /// Brace-enclosed Initializer List
    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : Red(r), Green(g), Blue(b), Alpha(a) {}

    /// Cast from uint32_t
    constexpr Color(uint32_t val)
    : Color ((uint8_t) (((val >> 0x00) & 0x1F) << 3),
             (uint8_t) (((val >> 0x08) & 0x1F) << 3),
             (uint8_t) (((val >> 0x10) & 0x1F) << 3),
             (uint8_t) (((val >> 0x18) & 0x1F) << 3)) {}
};

#pragma pack(push, 2)
struct BitmapHeader
{
    uint16_t    Type;       // Always 0x4D42
    uint32_t    FileSize;
    uint16_t    Reserved1;
    uint16_t    Reserved2;
    uint32_t    OffBits;    // Pointer to the bytes
};

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

struct Batch
{
    Quad        DstQuad;
    Rect        SrcRect;
    int8_t      Group;
    uint8_t     PalDex;
    uint8_t     BlendMode;
};

class Drawer
{
    private:

    SDL_Window* _window;
    SDL_GLContext _glContext;

    bool _ready2Draw = false;

    float _bgColor[3] = { 0, 0, 0 };
    bool _drawOvr =  false;
    float _ovrColor[16] =
    {
        0, 0, 0, 2,   // TL
        0, 0, 0, 2,   // TR
        0, 0, 0, 2,   // BL
        0, 0, 0, 2    // BR
    };

    std::vector<Vertex> _drawQueue;
    std::vector<uint32_t> _indexQueue;

    GLuint _mainShader, _overlayShader, _screenShader, _frameBuffer, _frameTexID, _atlasTexID, _palTexID,
           _VAO, _VBO, _EBO, _VBOSize, _screenVAO, _screenVBO;
    GLint  _ovrLoc, _screenLoc, _atlasLoc, _paletteLoc;

    int _winW, _winH;

    const glm::mat4 _projection = glm::ortho(0.0f, 320.0f, 240.0f, 0.0f, -1.0f, 1.0f);

    const float _invAtlasWidth = 1.0f / 256.0f;
    const float _invAtlasHeight = 1.0f / 5632.0f;

    const uint16_t _atlasOffsetY[6] = { 0x0000, 0x0800, 0x0D00,
                                        0x1300, 0x1400, 0x1500 };
    const uint16_t _texMaxHeight[6] = { 0x0800, 0x0500, 0x0600,
                                        0x0100, 0x0100, 0x0100 };

    bool _fullscreen, _aspectRatio;

    /// Compile a shader
    void CompileShader(GLuint& shader, const char* vertShader, const char* fragShader);

    /// Checks for compilation errors while creating shaderProg
    bool CheckShaderCompile(GLuint shader, const char* shaderName);

    /// Checks for linking issues with shaderProg
    bool CheckProgramLink(GLuint program);

    /// Catches usual OpenGL errors
    bool CheckGLError(const char* errorMessage);

    /// Checks for OpenGL extensions
    bool CheckExtension(const char* extName);

    public:

    /// Constructor
    Drawer();

    bool IsReady() { return _ready2Draw; };
    void SetReady(bool val) { _ready2Draw = val; };

    /// Initialize SDL, create window, OpenGL context, compile shaders, allocate textures
    bool Init();

    /// Set the background color for the frame
    void SetBGColor(const Color& color)
    { _bgColor[0] = color.Red / 255.0f;
      _bgColor[1] = color.Green / 255.0f;
      _bgColor[2] = color.Blue / 255.0f; }

    /// Enable the overlay
    void EnableOvr(bool val) { _drawOvr = val; };

    /// Update the overlay
    void UpdateOverlay(float* rec) { memcpy(_ovrColor, rec, 16 * sizeof(float)); };

    /// Draw the actual frame and render to the screen
    void DrawFrame();

    /// Clean the queues
    void ClearQueues();

    /// Resize the window
    void ResizeWindow(int width, int height);

    /// Toggle fullscreen mode
    void ToggleFullscreen();

    /// Toggle preservation of aspect ratio
    void ToggleAspectRatio();

    /// Load a texture sheet into the atlas
    bool LoadTexture(const uint8_t* texData, uint16_t texHeight, int group);

    /// Load a group of palettes into the palette texture
    bool LoadPalette(const uint8_t* palData, uint16_t palHeight, int group);

    /// Update the projection from the framebuffer to the screen
    void UpdateScreenProjection();

    /// Add a batch to the current draw queue
    void AddBatch(const Batch& b);

    /// Expose the SDL_Window pointer
    SDL_Window* GetWindow() const { return _window; }

    /// Destructor
    ~Drawer();
};

class Palette
{
    private:

    uint8_t     _palCount;
    Color*      _data;

    public:

    /// Constructor
    Palette() : _palCount(0), _data(nullptr) {};

    /// Set the palettes
    int PalFromData(uint16_t* src, int palCount);

    /// Destructor
    ~Palette() { free(_data); };

    // Getters
    uint8_t*    GetData()   { return (uint8_t*) _data; }
    uint8_t     GetCount()  { return _palCount; }
};

class Texture
{
    private:

    uint32_t    _size;
    uint32_t    _width;
    uint32_t    _height;
    Palette*    _pal;
    char*       _dataBits;

    /// Swap uint8_t nibbles
    inline uint8_t bswap(uint8_t x) { return (uint8_t) ((x << 4) | (x >> 4)); }

    /// Unzip dictionnary encoded texture
    uint32_t Unzip(char* src);

    public:

    /// Constructor
    Texture() : _size(0), _width(0), _height(0), _pal(nullptr), _dataBits(nullptr) {};

    /// Set the texture
    void TexFromData(char* texSrc, uint16_t* palSrc, uint32_t palSize);

    /// Destructor
    ~Texture() { free(_dataBits);
                 delete _pal; };

    // Getters
    uint8_t*  GetDataBits()  { return (uint8_t*) _dataBits; }
    uint32_t  GetHeight()    { return _height; }
    Palette*  GetPalette()   { return _pal; }
};

#endif // IMAGINGE_H_INCLUDED


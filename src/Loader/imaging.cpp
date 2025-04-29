#include "imaging.h"
#include <cstring>
#include <cstdio>

/*  _____                                     _____  */
/* ( ___ )                                   ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |  ____                               |   |  */
/*  |   | |  _ \ _ __ __ ___      _____ _ __  |   |  */
/*  |   | | | | | '__/ _` \ \ /\ / / _ \ '__| |   |  */
/*  |   | | |_| | | | (_| |\ V  V /  __/ |    |   |  */
/*  |   | |____/|_|  \__,_| \_/\_/ \___|_|    |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                                   (_____) */

const char* mainVertShader = R"(
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in int aGroup;
layout (location = 3) in int aPalDex;
layout (location = 4) in int aBlendMode;

out vec2 vTexCoord;
flat out int vGroup;
flat out int vPalDex;
flat out int vBlendMode;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    vTexCoord = aTexCoord;
    vGroup = aGroup;
    vPalDex = aPalDex;
    vBlendMode = aBlendMode;
}   )";

const char* mainFragShader = R"(
#version 330 core

in vec2 vTexCoord;
flat in int vGroup;
flat in int vPalDex;
flat in int vBlendMode;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BlendFact;

uniform usampler2D uAtlas;
uniform sampler2D uPalette;

const float invPalW = 1.0 / 96.0;
const float invPalH = 1.0 / 32.0;

const vec2 blendModes[5] = vec2[]
(
    vec2(1.0, 0.0),  // Disabled
    vec2(0.5, 0.5),  // Mean
    vec2(1.0, 1.0),  // Additive
    vec2(-1.0, 1.0), // Subtractive
    vec2(0.25, 1.0)  // Overlay
);

void main()
{
    // Get the palette index
    float logicalX = vTexCoord.x * 256;
    int nibbleMask = int((logicalX - 2 * floor(logicalX * 0.5)) < 1.0);
    uvec4 atlasSample = texture(uAtlas, vTexCoord);
    uint byteVal = atlasSample.r;
    uint index = (byteVal >> (nibbleMask * 4)) & 0xFu;

    // Get the color
    float palU = (float(vGroup) * 16 + float(index) + 0.5) * invPalW;
    float palV = (float(vPalDex) + 0.5) * invPalH;
    vec4 color = texture(uPalette, vec2(palU, palV));

    // Handle transparency
    bool STPoff = color.a == 0.0;
    bool isBlack = color.r == 0.0 && color.g == 0.0 && color.b == 0.0;
    if (STPoff && isBlack) discard;
    vec2 blend = blendModes[int(STPoff) * vBlendMode];

    FragColor = blend.x * color;
    BlendFact = vec4(blend.y);
}   )";

const char* screenVertShader = R"(
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 vTexCoord;

uniform mat4 uProjection;

void main()
{
    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
    vTexCoord = aTexCoord;
}   )";

const char* screenFragShader = R"(
#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uScreenTexture;

void main()
{
    FragColor = texture(uScreenTexture, vTexCoord);
}   )";

Drawer::Drawer()
    : _window(nullptr), _glContext(nullptr),
      _mainShader(0), _screenShader(0), _frameBuffer(0),
      _winW(0), _winH(0), _fullscreen(false), _aspectRatio(true) {}

bool Drawer::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL Initialization Error: %s\n", SDL_GetError());
        return false;
    }

    // Request an OpenGL 3.3 Core context and create a resizable window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    _window = SDL_CreateWindow("The Adventures of Alundra", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               672, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!_window)
    {
        fprintf(stderr, "Window Creation Error: %s\n", SDL_GetError());
        return false;
    }

    _glContext = SDL_GL_CreateContext(_window);

    if (!_glContext)
    {
        fprintf(stderr, "OpenGL Context Creation Error: %s\n", SDL_GetError());
        return false;
    }

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW Initialization Error\n");
        return false;
    }

    if (!CheckExtension("GL_ARB_blend_func_extended"))
    {
        printf("Dual-Source Blending is NOT supported on this GPU\n");
        return false;
    }

    CompileShaders();

    // Set projections
    UpdateScreenProjection();
    glUseProgram(_mainShader);
    glUniformMatrix4fv(glGetUniformLocation(_mainShader, "uProjection"), 1, GL_FALSE, &_projection[0][0]);

    // Create the framebuffer
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);

    // Allocate the framebuffer texture
    glGenTextures(1, &_frameTexID);
    glBindTexture(GL_TEXTURE_2D, _frameTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 336, 256, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Link the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _frameTexID, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "Error: Deficient Framebuffer\n");
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Allocate the atlas texture
    // The logical atlas size is 256x5632, but due to 2 pixels per byte format, we allocate width = 128
    glGenTextures(1, &_atlasTexID);
    glBindTexture(GL_TEXTURE_2D, _atlasTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 128, 5632, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (CheckGLError("Failed to allocate atlas texture")) return false;

    // Allocate the palette texture
    // Palette texture size is 96x32 (6 groups containing a maximum of 32 palettes, 16 pixels wide)
    glGenTextures(1, &_palTexID);
    glBindTexture(GL_TEXTURE_2D, _palTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 96, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (CheckGLError("Failed to allocate palette texture")) return false;

    // Create and set screenVAO and screenVBO
    float quadVertices[] =
    {
        // Positions          // TexCoords
          0.0f,    0.0f,      0.0f, 1.0f,
        336.0f,    0.0f,      1.0f, 1.0f,
        336.0f,  256.0f,      1.0f, 0.0f,

          0.0f,    0.0f,      0.0f, 1.0f,
        336.0f,  256.0f,      1.0f, 0.0f,
          0.0f,  256.0f,      0.0f, 0.0f
    };

    glGenVertexArrays(1, &_screenVAO);
    glGenBuffers(1, &_screenVBO);
    glBindVertexArray(_screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

    // Main buffers creation
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    // Set main VBO memory
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

    _VBOSize = _drawQueue.capacity() * sizeof(Vertex);
    glBufferData(GL_ARRAY_BUFFER, _VBOSize, nullptr, GL_STREAM_DRAW);

    // Set locations
    _screenLoc = glGetUniformLocation(_screenShader, "uScreenTexture");
    _atlasLoc = glGetUniformLocation(_mainShader, "uAtlas");
    _paletteLoc = glGetUniformLocation(_mainShader, "uPalette");

    // Define vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, X));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, U));
    glVertexAttribIPointer(2, 1, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, Group));
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, PalDex));
    glVertexAttribIPointer(4, 1, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, BlendMode));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Set up blending functions
    glBlendFunc(GL_ONE, GL_SRC1_COLOR);
    glBlendEquation(GL_FUNC_ADD);

    return true;
}

void Drawer::DrawFrame()
{
    //--------------------//
    //     FIRST PASS     //
    //--------------------//

    // Bind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glViewport(0, 0, 336, 256);

    // Set background color
    glClearColor(_bgColor[0], _bgColor[1], _bgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(_mainShader);

    // Bind the atlas to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _atlasTexID);
    glUniform1i(_atlasLoc, 0);

    // Bind the palette to unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _palTexID);
    glUniform1i(_paletteLoc, 1);

    // Enable dual-source blending
    glEnable(GL_BLEND);

    if (!_drawQueue.empty())
    {
        // Bind VAO, VBO and EBO
        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

        // Reallocate the VBO if it changed in size
        size_t queueSize = _drawQueue.size() * sizeof(Vertex);
        if (queueSize > _VBOSize)
        {
            _VBOSize = queueSize;
            glBufferData(GL_ARRAY_BUFFER, _VBOSize, nullptr, GL_STREAM_DRAW);
        }

        // Send the vertices to the GPU
        void* ptr = glMapBufferRange(GL_ARRAY_BUFFER, 0, queueSize, GL_MAP_WRITE_BIT);
        if (ptr)
        {
            memcpy(ptr, _drawQueue.data(), queueSize);
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        // Update the EBO
        size_t indexSize = _indexQueue.size() * sizeof(uint32_t);
        if (indexSize > 0)
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, _indexQueue.data(), GL_STREAM_DRAW);
        }

        // Draw the quads
        glDrawElements(GL_TRIANGLES, _indexQueue.size(), GL_UNSIGNED_INT, 0);
    }

    // Disable the blending
    glDisable(GL_BLEND);

    //---------------------//
    //     SECOND PASS     //
    //---------------------//

    // Back to default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _winW, _winH);

    // Fill the rest of the window with black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(_screenShader);

    // Bind the frame to unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _frameTexID);
    glUniform1i(_screenLoc, 0);

    // Render to the screen
    glBindVertexArray(_screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Clean the queues
    _drawQueue.clear();
    _indexQueue.clear();
}

void Drawer::CompileShaders()
{
    // Compile and link main shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &mainVertShader, nullptr);
    glCompileShader(vertexShader);

    if (!CheckShaderCompile(vertexShader, "Vertex Shader"))
    {
        fprintf(stderr, "Vertex shader failed to compile\n");
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &mainFragShader, nullptr);
    glCompileShader(fragmentShader);

    if (!CheckShaderCompile(fragmentShader, "Fragment Shader"))
    {
        fprintf(stderr, "Fragment shader failed to compile\n");
    }

    _mainShader = glCreateProgram();
    glAttachShader(_mainShader, vertexShader);
    glAttachShader(_mainShader, fragmentShader);
    glLinkProgram(_mainShader);

    if (!CheckProgramLink(_mainShader))
    {
        fprintf(stderr, "Shader program failed to link\n");
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Compile and link screen shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &screenVertShader, nullptr);
    glCompileShader(vertexShader);

    if (!CheckShaderCompile(vertexShader, "Vertex Shader"))
    {
        fprintf(stderr, "Vertex shader failed to compile\n");
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &screenFragShader, nullptr);
    glCompileShader(fragmentShader);

    if (!CheckShaderCompile(fragmentShader, "Fragment Shader"))
    {
        fprintf(stderr, "Fragment shader failed to compile\n");
    }

    _screenShader = glCreateProgram();
    glAttachShader(_screenShader, vertexShader);
    glAttachShader(_screenShader, fragmentShader);
    glLinkProgram(_screenShader);

    if (!CheckProgramLink(_screenShader))
    {
        fprintf(stderr, "Shader program failed to link\n");
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Drawer::ToggleFullscreen()
{
    _fullscreen = !_fullscreen;
    SDL_SetWindowFullscreen(_window, _fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    SDL_GL_GetDrawableSize(_window, &_winW, &_winH);
    glViewport(0, 0, _winW, _winH);
    UpdateScreenProjection();
}

void Drawer::ToggleAspectRatio()
{
    _aspectRatio = !_aspectRatio;
    UpdateScreenProjection();
}

void Drawer::ResizeWindow(int width, int height)
{
    SDL_SetWindowSize(_window, width, height);
    UpdateScreenProjection();
    glViewport(0, 0, _winW, _winH);
}

bool Drawer::LoadTexture(const uint8_t* texData, uint16_t texHeight, int group)
{
    if (group < 0 || group >= 6)
    {
        printf("Error: Invalid texture index\n");
        return false;
    }

    if (texHeight > _texMaxHeight[group])
    {
        printf("Error: Invalid texture height\n");
        return false;
    }

    // 4bpp data row = 128 bytes -> Logical width = 256 pixels
    glBindTexture(GL_TEXTURE_2D, _atlasTexID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, _atlasOffsetY[group], 128, texHeight, GL_RED_INTEGER, GL_UNSIGNED_BYTE, texData);

    return !CheckGLError("Failed to load texture into atlas");
}

bool Drawer::LoadPalette(const uint8_t* palData, uint16_t palHeight, int group)
{
    if (group < 0 || group >= 6)
    {
        printf("Error: Invalid palette index\n");
        return false;
    }

    if (palHeight > 32)
    {
        printf("Error: Invalid palette height\n");
        return false;
    }

    // The palette texture is 96x32; we load a block of 16x32 from palData
    int offsetX = group << 4;
    glBindTexture(GL_TEXTURE_2D, _palTexID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, offsetX, 0, 16, palHeight, GL_RGBA, GL_UNSIGNED_BYTE, palData);

    return !CheckGLError("Failed to load palette group");
}

void Drawer::UpdateScreenProjection()
{
    SDL_GL_GetDrawableSize(_window, &_winW, &_winH);

    const float nativeW = 336.0f;
    const float nativeH = 256.0f;

    float scaleX = float(_winW) / nativeW;
    float scaleY = float(_winH) / nativeH;

    glm::mat4 ortho = glm::ortho(0.0f, float(_winW), float(_winH), 0.0f, -1.0f, 1.0f);
    glm::mat4 model;

    if (_aspectRatio)
    {
        float scale = (scaleX < scaleY ? scaleX : scaleY);
        float offsetX = (_winW - nativeW * scale) * 0.5f;
        float offsetY = (_winH - nativeH * scale) * 0.5f;

        model = glm::translate(glm::mat4(1.0f), glm::vec3(offsetX, offsetY, 0.0f))
              * glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));
    }
    else
    {
        model = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, 1.0f));
    }

    glm::mat4 projection = ortho * model;

    // Update projection uniform
    glUseProgram(_screenShader);
    glUniformMatrix4fv(glGetUniformLocation(_screenShader, "uProjection"), 1, GL_FALSE, &projection[0][0]);
}

void Drawer::AddBatch(const Quad& dstQuad, const Rect& srcRect, int group, int palDex, int blendMode)
{
    float u0 = srcRect.X * _invAtlasWidth;
    float u1 = (srcRect.X + srcRect.Width) * _invAtlasWidth;
    float v0 = (_atlasOffsetY[group] + srcRect.Y) * _invAtlasHeight;
    float v1 = (_atlasOffsetY[group] + srcRect.Y + srcRect.Height) * _invAtlasHeight;

    size_t baseIndex = _drawQueue.size();

    _drawQueue.push_back(Vertex { dstQuad.V0.X, dstQuad.V0.Y, u0, v0, group, palDex, blendMode });
    _drawQueue.push_back(Vertex { dstQuad.V1.X, dstQuad.V1.Y, u1, v0, group, palDex, blendMode });
    _drawQueue.push_back(Vertex { dstQuad.V2.X, dstQuad.V2.Y, u1, v1, group, palDex, blendMode });
    _drawQueue.push_back(Vertex { dstQuad.V3.X, dstQuad.V3.Y, u0, v1, group, palDex, blendMode });

    // Add indices for the EBO
    _indexQueue.push_back(baseIndex + 0);
    _indexQueue.push_back(baseIndex + 1);
    _indexQueue.push_back(baseIndex + 3);
    _indexQueue.push_back(baseIndex + 1);
    _indexQueue.push_back(baseIndex + 2);
    _indexQueue.push_back(baseIndex + 3);
}

bool Drawer::CheckShaderCompile(GLuint shader, const char* shaderName)
{
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        fprintf(stderr, "%s Compilation Error: %s\n", shaderName, log);
        return false;
    }

    return true;
}

bool Drawer::CheckProgramLink(GLuint program)
{
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        fprintf(stderr, "Shader Program Linking Error: %s\n", log);
        return false;
    }

    return true;
}

bool Drawer::CheckGLError(const char* errorMessage)
{
    GLenum err = glGetError();

    if (err != GL_NO_ERROR)
    {
        fprintf(stderr, "%s OpenGL Error: %d\n", errorMessage, err);
        return true;
    }

    return false;
}

bool Drawer::CheckExtension(const char* extName)
{
    GLint numExt = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);

    for (GLint i = 0; i < numExt; i++)
    {
        const char* ext = (const char*) glGetStringi(GL_EXTENSIONS, i);

        if (strcmp(ext, extName) == 0)
        {
            return true;
        }
    }

    return false;
}

Drawer::~Drawer()
{
    if (_screenShader) glDeleteProgram(_screenShader);
    if (_mainShader) glDeleteProgram(_mainShader);
    if (_frameTexID) glDeleteTextures(1, &_frameTexID);
    if (_atlasTexID) glDeleteTextures(1, &_atlasTexID);
    if (_palTexID) glDeleteTextures(1, &_palTexID);
    if (_frameBuffer) glDeleteFramebuffers(1, &_frameBuffer);
    if (_glContext) SDL_GL_DeleteContext(_glContext);
    if (_window) SDL_DestroyWindow(_window);
    if (_EBO) glDeleteBuffers(1, &_EBO);
    if (_VBO) glDeleteBuffers(1, &_VBO);
    if (_VAO) glDeleteVertexArrays(1, &_VAO);
    if (_screenVBO) glDeleteBuffers(1, &_screenVBO);
    if (_screenVAO) glDeleteVertexArrays(1, &_screenVAO);
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

int Palette::PalFromData(uint16_t* src, int palCount)
{
    _palCount = palCount;

    _data = (Color*) malloc(_palCount << 6);

    if (!_data)
    {
        fprintf(stderr, "Memory allocation failed: in Palette::PalFromData()\n");
        return 1;
    }

    uint16_t clr = 0;

    for (int y = 0; y < _palCount; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            clr = *(src + (y << 4) + x);
            Color rgba = Color { (char) ((clr & 0x001F) << 3),
                                 (char) ((clr & 0x03E0) >> 2),
                                 (char) ((clr & 0x7C00) >> 7),
                                 (char) ((clr >> 15) * 0xFF) };
            _data[(y << 4) + x] = rgba;
        }
    }

    return 0;
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

uint32_t Texture::Unzip(char* src)
{
    uint32_t rhead = 6;
    uint32_t whead = 0;
    uint32_t seek = 0;
    uint8_t cur = 0;
    uint8_t dist = 0;
    uint8_t len = 0;
    bool keep = true;

    // Checks if uncompressed
    if (*((uint16_t*) src) != 0x5A45)
    {
        size_t sheet = 0x8000;
        _dataBits = (char*) malloc(sheet);
        memcpy(_dataBits, src, sheet);

        for (int i = 0; i < sheet; i++)
        {
            _dataBits[i] = bswap(_dataBits[i]);
        }

        _width = 256;
        _height = sheet >> 7;

        return sheet;
    }

    char* buffer = (char*) malloc(0x80000);

    // Unzip
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

    if (whead > 0)
    {
        _width = 256;
        _height = whead >> 7;

        _dataBits = (char*) realloc(buffer, whead);
    }
    else free(buffer);

    return whead;
}

void Texture::TexFromData(char* texSrc, uint16_t* palSrc, uint32_t palSize)
{
    // Generate the palette
    _pal = new Palette();
    _pal->PalFromData(palSrc, palSize >> 5);

    // Dictionary decoding algorithm
    _size = Unzip(texSrc);
}

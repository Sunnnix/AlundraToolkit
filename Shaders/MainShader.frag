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
}
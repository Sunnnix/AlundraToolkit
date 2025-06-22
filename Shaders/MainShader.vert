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
}
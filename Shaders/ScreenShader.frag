#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D uScreenTexture;

void main()
{
    FragColor = texture(uScreenTexture, vTexCoord);
}
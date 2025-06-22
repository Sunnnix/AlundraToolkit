#version 330 core
in vec2 oTex;

out vec4 FragColor;

uniform sampler2D tex;

void main(){
    vec4 color = texture(tex, oTex);// * oColor;
	if (color.a == 0)
		discard;
	FragColor = color;
}
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

out vec2 oTex;

//uniform mat4 projection;
//uniform mat4 model;

void main() {
	gl_Position = /*projection * model * */vec4(aPos, 1.0, 1.0);
	oTex = aTex;
}
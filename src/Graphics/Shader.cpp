#include "Shader.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

/*  _____                                        _____  */
/* ( ___ )                                      ( ___ ) */
/*  |   |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|   |  */
/*  |   |    _____ _               _             |   |  */
/*  |   |   / ____| |             | |            |   |  */
/*  |   |  | (___ | |__   __ _  __| | ___ _ __   |   |  */
/*  |   |   \___ \| '_ \ / _` |/ _` |/ _ \ '__ | |   |  */
/*  |   |   ____) | | | | (_| | (_| |  __/ |     |   |  */
/*  |   |  |_____/|_| |_|\__,_|\__,_|\___|_|     |   |  */
/*  |___|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|___|  */
/* (_____)                                      (_____) */

static int success;
static char infoLog[512];

int createShader(int type, std::string path);

static GLuint latestShader;

Shader::Shader(std::string path) {
	ID = 0;
	int vert, frag;

	if (!(vert = createShader(0, path)))
		return;
	if (!(frag = createShader(1, path))) {
		glDeleteShader(vert);
		return;
	}

	ID = glCreateProgram();
	glAttachShader(ID, vert);
	glAttachShader(ID, frag);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "Linking Program for " << path << " failed\n" << infoLog << std::endl;
		glDeleteProgram(ID);
		ID = 0;
	}
	
	glDeleteShader(vert);
	glDeleteShader(frag);
}

void Shader::bind() {
	if (latestShader == ID)
		return;
	latestShader = ID;
	glUseProgram(ID);
}

void Shader::unbind() {
	if (latestShader == 0)
		return;
	latestShader = 0;
	glUseProgram(0);
}

unsigned int Shader::getUniformLocation(std::string& name) {
	auto [it, inserted] = locations.try_emplace(name, glGetUniformLocation(ID, name.c_str()));
	return it->second;
}

void Shader::uniformInt(std::string name, int value) {
	bind();
	glUniform1i(getUniformLocation(name), value);
}

void Shader::uniformMat4(std::string name, const glm::mat4& matrix) {
	bind();
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

Shader::~Shader() {
	glDeleteProgram(ID);
}

int createShader(int type, std::string path) {
	int id;
	std::string sPath;
	switch (type) {
	case 0:
		id = glCreateShader(GL_VERTEX_SHADER);
		sPath = path + ".vert";
		break;
	case 1:
		id = glCreateShader(GL_FRAGMENT_SHADER);
		sPath = path + ".frag";
		break;
	default:
		std::cerr << "Error creating Shader for " << path << " (invalid type " << type << ")" << std::endl;
		return 0;
	}
	std::ifstream file(sPath);
	if (file.bad()) {
		std::cerr << "Error creating Shader for " << sPath << " (error opening file)" << std::endl;
		return 0;
	}
	std::ostringstream ss;
	ss << file.rdbuf();
	std::string src = ss.str();
	if (src.length() == 0) {
		std::cerr << "Shader source for " << sPath << " is empty!\n" << infoLog << std::endl;
		return 0;
	}
	const char* cStr = src.c_str();
	glShaderSource(id, 1, &cStr, NULL);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		std::cerr << "Error creating Shader for " << sPath << " (compiling error)\n" << infoLog << std::endl;
		return 0;
	}

	return id;
}
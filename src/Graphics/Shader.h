#pragma once
#include <string>
#include <unordered_map>
#include <glm/ext/matrix_float4x4.hpp>

class Shader {
private:
	int ID;
	std::unordered_map<std::string, unsigned int> locations;
public:
	Shader(std::string path);

	void bind();
	void uniformInt(std::string name, int value);
	void uniformMat4(std::string name, const glm::mat4& matrix);
	static void unbind();

	Shader(Shader&) = delete;
	Shader operator=(Shader&) = delete;

	~Shader();
private:
	unsigned int getUniformLocation(std::string& name);
};
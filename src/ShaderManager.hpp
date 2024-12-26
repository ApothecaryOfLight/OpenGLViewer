#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <vector>

#include "ShaderLoader.hpp"

class ShaderManager {
public:
	ShaderManager();
	GLuint loadShader();
	GLuint loadShader(const GLchar* inVertexShaderSource, const GLchar* inFragmentShaderSource);
	void loadShaders();
	void loadShadersLight();
	void loadShadersLightCel();
	void loadShaderByFile(std::string inFileLoc);
	GLuint getShader(size_t inShaderHashKey);

	size_t initializeShader(std::string inShaderFile);
	GLuint compileShader(size_t inShaderHashKeyID);

	GLuint loadMyShader();
	// Shader sources
	const GLchar* vertexSource;
	const GLchar* fragmentSource;

	ShaderLoader *myShaderLoader;

	std::vector<std::pair<std::string,size_t>> getShaderList();
private:
	std::unordered_map<size_t,GLuint> myShaderPrograms;
};
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

#include "ConfigManager.hpp"
#include "FilepathManager.hpp"


class ShaderManager {
public:
	ShaderManager(ConfigManager* inConfigManager);
	void loadShaderByFile(std::string inFileLoc);
	void setShader(size_t inShaderHashKey);
	GLuint getShader(size_t inShaderHashKey);
	GLuint compileShader(size_t inShaderHashKeyID);
	std::vector<std::pair<std::string,size_t>> getShaderList();

	GLuint myCurrentShaderProgramID;
private:
	std::unordered_map<size_t,GLuint> myShaderPrograms;
	FilepathManager* myFilepathManager;
	ConfigManager* myConfigManager;
	ShaderLoader *myShaderLoader;
};
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderManager.hpp"

#include "FilepathManager.hpp"

#include <string>

#include "ShaderLoader.hpp"

#include "ConfigManager.hpp"

#include <iostream>

ShaderManager::ShaderManager(ConfigManager* inConfigManager) {
    std::cout << "ShaderManager constructor called!" << std::endl;
    myConfigManager = inConfigManager;
    myShaderLoader = new ShaderLoader();
    myFilepathManager = myConfigManager->myFilepathManager;

    std::string myShaderDirectory(myFilepathManager->myShaderDir);

    for( auto& ShaderFilePath : myConfigManager->myShaders ) {
        loadShaderByFile(myShaderDirectory + ShaderFilePath);
    }

    //Set current shader to the default specified in the config.xml file.
    myCurrentShaderProgramID = std::hash<std::string>{}(myConfigManager->myDefaultShader);
    std::cout << "Setting default shader of " << myConfigManager->myDefaultShader << " to " << myCurrentShaderProgramID << std::endl;
    setShader( 8343586746150268247 ); //Hacky solution until config.xml is being read by tinyXML, see if the error persists then.
}

void ShaderManager::loadShaderByFile(std::string inFileLoc) {
    std::cout << "Loading shader " << inFileLoc << std::endl;
    size_t myShaderHashKey = myShaderLoader->loadShaderData(inFileLoc);
    myShaderPrograms[myShaderHashKey] = compileShader(myShaderHashKey);
}

GLuint ShaderManager::compileShader(size_t inShaderHashKeyID) {
    std::cout << "Compiling shader " << inShaderHashKeyID << std::endl;
    ShaderData *myShaderData = myShaderLoader->getShader(inShaderHashKeyID);
    const GLchar* myLocalVertexSource = myShaderData->vertexSource.c_str();
    const GLchar* myLocalFragmentSource = myShaderData->fragmentSource.c_str();

    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &myLocalVertexSource, NULL);
    glCompileShader(vertexShader);

    // Check for compile errors
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
        std::cout << myShaderData->vertexSource.c_str() << std::endl << std::endl;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &myLocalFragmentSource, NULL);
    glCompileShader(fragmentShader);

    // Check for compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Error: Fragment shader compilation failed\n" << infoLog << std::endl;
    }

    // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
        std::cout << "VERTEX: " << std::endl << myShaderData->vertexSource.c_str() << std::endl << std::endl;
        std::cout << "FRAGMENT: " << std::endl << myShaderData->fragmentSource.c_str() << std::endl << std::endl;
    }

    glUseProgram(shaderProgram);

    // Cleanup the shader setup information
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

std::vector<std::pair<std::string,size_t>> ShaderManager::getShaderList() {
    return myShaderLoader->getShaderList();
}

const char* GetGLErrorString(GLenum error) {
    switch (error) {
        case GL_NO_ERROR: return "No error";
        case GL_INVALID_ENUM: return "Invalid enum";
        case GL_INVALID_VALUE: return "Invalid value";
        case GL_INVALID_OPERATION: return "Invalid operation";
        case GL_STACK_OVERFLOW: return "Stack overflow";
        case GL_STACK_UNDERFLOW: return "Stack underflow";
        case GL_OUT_OF_MEMORY: return "Out of memory";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "Invalid framebuffer operation";
        default: return "Unknown error";
    }
}

void ShaderManager::setShader(size_t inShaderHashKey) {
    //std::cout << "Switching to shader with id " << inShaderHashKey << std::endl;
    //Get the shader program.
    //std::cout << "Using shader hash key " << inShaderHashKey << std::endl;
    GLuint myShaderProgram = myShaderPrograms[inShaderHashKey];
    //std::cout << "myShaderProgram to set: " << myShaderProgram << std::endl;

    //Validate the shader to make sure it's valid.
    glValidateProgram(myShaderProgram);
    GLint validateStatus;
    glGetProgramiv(myShaderProgram, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE) {
        char infoLog[512];
        glGetProgramInfoLog(myShaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Program validation failed\n" << infoLog << std::endl;
    } /*else {
        //std::cout << "Program validated successfully!" << std::endl;
    }
    std::cout << "Using new shader program!" << std::endl;*/

    //Switch to this shader.
    glUseProgram(myShaderProgram);

    //Check for glUseProgram errors.
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << GetGLErrorString(error) << std::endl;
    } /*else {
        std::cout << "No OpenGL errors detected." << std::endl;
    }*/

    GLint modelLocation = glGetUniformLocation(myShaderProgram, "model");
    if (modelLocation == -1) {
        std::cerr << "Uniform 'model' not found in shader program!" << std::endl;
    }
    GLint viewLocation = glGetUniformLocation(myShaderProgram, "view");
    if (viewLocation == -1) {
        std::cerr << "Uniform 'view' not found in shader program!" << std::endl;
    }
    GLint projLocation = glGetUniformLocation(myShaderProgram, "proj");
    if (projLocation == -1) {
        std::cerr << "Uniform 'proj' not found in shader program!" << std::endl;
    }

    myCurrentShaderProgramID = myShaderPrograms[inShaderHashKey];
}

GLuint ShaderManager::getShader(size_t inShaderHashKey) {
    return myShaderPrograms[inShaderHashKey];
}
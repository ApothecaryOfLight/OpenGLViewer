#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderManager.hpp"


#include "ShaderLoader.hpp"

#include <iostream>

ShaderManager::ShaderManager() {
    myShaderLoader = new ShaderLoader();
    loadShaderByFile("../../bin/data/shaders/simple_light.shaderxml");
    loadShaderByFile("../../bin/data/shaders/plain_materials.shaderxml");
    loadShaderByFile("../../bin/data/shaders/cel.shaderxml");
}

void ShaderManager::loadShaderByFile(std::string inFileLoc) {
    std::cout << "Loading shader " << inFileLoc << std::endl;
    size_t myShaderHashKey = myShaderLoader->loadShaderData(inFileLoc);
    myShaderPrograms[myShaderHashKey] = compileShader(myShaderHashKey);
}

GLuint ShaderManager::loadShader() {
    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Check for compile errors
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
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
    }

    glUseProgram(shaderProgram);

    // Cleanup the shader setup information
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

GLuint ShaderManager::loadShader(const GLchar* inVertexShaderSource, const GLchar* inFragmentShaderSource) {
    // Create and compile the vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &inVertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check for compile errors
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Error: Vertex shader compilation failed\n" << infoLog << std::endl;
    }

    // Create and compile the fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &inFragmentShaderSource, NULL);
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

    //  Check for linking errors.
    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Program linking failed\n" << infoLog << std::endl;
    }

    // Check for linking errors. Again.
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Shader program linking failed\n" << infoLog << std::endl;
    }

    glUseProgram(shaderProgram);

    // Cleanup the shader setup information
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::cout << "Returning GLuint for shader as " << shaderProgram << std::endl;
    return shaderProgram;
}

GLuint ShaderManager::loadMyShader() {
    loadShader(
        R"glsl(
            #version 150 core
            in vec3 position;
            uniform mat4 model;
            uniform mat4 view;
            uniform mat4 proj;
            void main() {
                gl_Position = proj * view * model * vec4(position, 1.0);
            }
        )glsl"
        ,
        R"glsl(
        #version 150 core
        out vec4 outColor;
        void main() {
            outColor = vec4(0.0, 1.0, 0.0, 1.0);
        }
    )glsl"
    );
}

void ShaderManager::loadShaders() {
    // Shader sources
    vertexSource = R"glsl(
    #version 150 core
    in vec3 position;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    void main() {
        gl_Position = proj * view * model * vec4(position, 1.0);
    }
)glsl";

    fragmentSource = R"glsl(
    #version 150 core
    out vec4 outColor;
    void main() {
        outColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
)glsl";
}


void ShaderManager::loadShadersLight() {
    // Shader sources
    vertexSource = R"glsl(
    #version 150 core
    in vec3 position;
    in vec3 normal;
    out vec3 FragPos;
    out vec3 Normal;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    void main() {
        FragPos = vec3(model * vec4(position, 1.0));
        Normal = mat3(transpose(inverse(model))) * normal;  
        gl_Position = proj * view * model * vec4(position, 1.0);
    }
)glsl";


    fragmentSource = R"glsl(
    #version 150 core
    in vec3 FragPos;
    in vec3 Normal;
    out vec4 outColor;
    struct Light {
        vec3 position;
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    struct Material {
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
    };
    uniform Light light;
    uniform Material material;
    uniform vec3 viewPos;
    void main() {
        // Ambient
        vec3 ambient = light.ambient * material.ambient;
        
        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * (diff * material.diffuse);
        
        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * (spec * material.specular);
        
        vec3 result = ambient + diffuse + specular;
        outColor = vec4(result, 1.0);
    }
)glsl";

}



void ShaderManager::loadShadersLightCel() {
    // Shader sources
    vertexSource = R"glsl(
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;  
    gl_Position = projection * view * model * vec4(position, 1.0);
}
)glsl";


    fragmentSource = R"glsl(
#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    // Ambient
    vec3 ambient = 0.1 * lightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Cel shading step
    if (diff > 0.95) diff = 1.0;
    else if (diff > 0.5) diff = 0.7;
    else if (diff > 0.25) diff = 0.4;
    else diff = 0.1;

    vec3 diffuse = diff * lightColor;
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}

)glsl";
}

size_t ShaderManager::initializeShader(std::string inShaderFile) {
    size_t myShaderHashKey = myShaderLoader->loadShaderData(inShaderFile);
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

GLuint ShaderManager::getShader(size_t inShaderHashKey) {
    //Get the shader program.
    std::cout << "Using shader hash key " << inShaderHashKey << std::endl;
    GLuint myShaderProgram = myShaderPrograms[inShaderHashKey];
    std::cout << "myShaderProgram to set: " << myShaderProgram << std::endl;

    //Validate the shader to make sure it's valid.
    glValidateProgram(myShaderProgram);
    GLint validateStatus;
    glGetProgramiv(myShaderProgram, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE) {
        char infoLog[512];
        glGetProgramInfoLog(myShaderProgram, 512, NULL, infoLog);
        std::cerr << "Error: Program validation failed\n" << infoLog << std::endl;
    } else {
        std::cout << "Program validated successfully!" << std::endl;
    }
    std::cout << "Using new shader program!" << std::endl;

    //Switch to this shader.
    glUseProgram(myShaderProgram);

    //Check for glUseProgram errors.
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << GetGLErrorString(error) << std::endl;
    } else {
        std::cout << "No OpenGL errors detected." << std::endl;
    }

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

    return myShaderPrograms[inShaderHashKey];
}
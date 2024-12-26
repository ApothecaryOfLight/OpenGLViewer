#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class ShapesManager {
public:
	ShapesManager();
	//GLfloat myVertices[9];
	//GLfloat myCubeVertices[108];
	GLfloat myLitCubeVertices[216];
};

class Shape {
public:
    virtual void initialize();
    virtual void setMaterial(GLuint shaderProgram);
    virtual void render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos, float angle);

    glm::vec3 myColor;
    glm::vec3 myPos;
    int triangles;
    bool isHighlighted;
private:
};

class Square : public Shape {
public:
    Square(glm::vec3 inColor, glm::vec3 Pos);
    GLuint VAO, VBO;
    GLfloat vertices[216];

    void initialize() override;
    void setMaterial(GLuint shaderProgram) override;
    void render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos, float angle) override;
};

class Plane : public Shape {
public:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    Plane(float size, int divisions, glm::vec3 inColor);
    void initialize() override;
    void setMaterial(GLuint shaderProgram) override;
    void render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos, float angle) override;
    std::vector<float> generateGridVertices(float size, int divisions);

    std::vector<unsigned int> generateGridIndices(int divisions);
};
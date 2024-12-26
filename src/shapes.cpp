#include "shapes.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

ShapesManager::ShapesManager() {  
}


Square::Square(glm::vec3 inColor, glm::vec3 inPos ) {
    myColor = inColor;
    myPos = inPos;
    GLfloat inVertices[216] = {
        // Positions       // Normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  -0.5f,  0.0f,  1.0f,  0.0f
    };
    for (int i = 0; i < 216; i++) {
        vertices[i] = inVertices[i];
    }
    triangles = 36;
}
void Shape::initialize() {
    isHighlighted = false;
}
void Shape::setMaterial(GLuint shaderProgram) {
}

void Shape::render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos, float angle) {
}

void Square::initialize() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    isHighlighted = false;
}

void Square::setMaterial(GLuint shaderProgram) {
    glm::vec3 yellow = myColor;
    if (isHighlighted) {
        yellow = glm::vec3(1.0f, 1.0f, 0.0f);
    }
    glm::vec3 ambient = myColor;
    glm::vec3 diffuse = yellow;
    glm::vec3 specular = myColor;
    float shininess = 32.0f;

    glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, glm::value_ptr(ambient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, glm::value_ptr(diffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(specular));
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), shininess);
}

void Square::render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos, float angle) {
    setMaterial(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, myPos);
    model = glm::rotate(model, angle, glm::vec3(1.0f, 1.0f, 1.0f));
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, triangles);
    glBindVertexArray(0);
}


Plane::Plane(float size, int divisions, glm::vec3 inColor ) {
    myColor = inColor;
    vertices = generateGridVertices(size, divisions);
    indices = generateGridIndices(divisions);
}

void Plane::initialize() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Plane::setMaterial(GLuint shaderProgram) {
    glm::vec3 ambient = myColor;
    glm::vec3 diffuse = myColor;
    glm::vec3 specular = myColor;
    float shininess = 32.0f;

    glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, glm::value_ptr(ambient));
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, glm::value_ptr(diffuse));
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, glm::value_ptr(specular));
    glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), shininess);
}

void Plane::render(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos, float angle) {
    setMaterial(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


std::vector<float> Plane::generateGridVertices(float size, int divisions) {
    std::vector<float> vertices;
    float step = size / divisions;
    for (int i = 0; i <= divisions; ++i) {
        for (int j = 0; j <= divisions; ++j) {
            vertices.push_back(j * step - size / 2); // x
            vertices.push_back(0.0f);                // y (height is 0)
            vertices.push_back(i * step - size / 2); // z
            vertices.push_back(0.0f);                // normal x
            vertices.push_back(1.0f);                // normal y
            vertices.push_back(0.0f);                // normal z
        }
    }
    return vertices;
}

std::vector<unsigned int> Plane::generateGridIndices(int divisions) {
    std::vector<unsigned int> indices;
    for (int i = 0; i < divisions; ++i) {
        for (int j = 0; j < divisions; ++j) {
            int start = i * (divisions + 1) + j;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + divisions + 1);
            indices.push_back(start + 1);
            indices.push_back(start + divisions + 2);
            indices.push_back(start + divisions + 1);
        }
    }
    return indices;
}



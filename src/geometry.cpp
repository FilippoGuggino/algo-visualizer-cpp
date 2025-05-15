#include <cmath>
#include <vector>
#include <numeric>

#include "geometry.h"
#include <glm/gtc/quaternion.hpp>

Rectangle::Rectangle(float width, float height)
{
    m_vertices = {
        width / 2, height / 2, 0.0f, // top right
        width / 2, -height / 2, 0.0f, // bottom right
        -width / 2, -height / 2, 0.0f, // bottom left
        -width / 2, height / 2, 0.0f // top left
    };

    m_indices = {
        0, 1, 3, // first triangle
        1, 2, 3 // second triangle
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Rectangle::draw(unsigned int shader)
{
    GLuint modelLoc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m_model[0][0]);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

static std::vector<GLfloat> cylinder_vertices(float height, float radius, int n_segments)
{
    std::vector<GLfloat> vertices;

    for (int i = 0; i < n_segments; i++) {
        float angle = 2.0f * M_PI * i / n_segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        // Bottom circle
        vertices.push_back(x);
        vertices.push_back(-height / 2.0f);
        vertices.push_back(z);

        // Top circle
        vertices.push_back(x);
        vertices.push_back(height / 2.0f);
        vertices.push_back(z);
    }

    return vertices;
}

Cylinder::Cylinder(float height, float radius, int n_segments)
    : m_model(glm::identity<glm::mat4>())
{
    m_vertices = cylinder_vertices(height, radius, n_segments);

    m_indices.resize(m_vertices.size() / 3);
    std::iota(std::begin(m_indices), std::end(m_indices), 0);
    m_indices.push_back(0);
    m_indices.push_back(1);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat), m_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
}

void Cylinder::draw(unsigned int shader)
{
    GLuint modelLoc = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &m_model[0][0]);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLE_STRIP, m_indices.size(), GL_UNSIGNED_INT, 0);
}

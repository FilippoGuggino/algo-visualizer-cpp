#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

#include "glad/glad.h"
#include <glm/glm.hpp>

class Geometry {
public:
    virtual void draw(unsigned int shader) = 0;
};

class Rectangle : public Geometry {
public:
    Rectangle(float width, float height);

    void draw(unsigned int shader);

private:
    std::vector<GLfloat> m_vertices;
    std::vector<GLuint> m_indices;

    glm::mat4 m_model;

    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
};

class Cylinder : public Geometry {
public:
    Cylinder(float height, float radius, int n_segments);

    void draw(unsigned int shader);

private:
    std::vector<GLfloat> m_vertices;
    std::vector<GLuint> m_indices;

    glm::mat4 m_model;

    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
};

#endif /* GEOMETRY_H */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

#include "glad/glad.h"

class Geometry {
public:
    virtual void draw() = 0;
};

class Rectangle : public Geometry {
public:
    Rectangle(float width, float height);

    void draw();

private:
    std::vector<GLfloat> m_vertices;
    std::vector<GLuint> m_indices;

    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
};

class Cylinder : public Geometry {
public:
    Cylinder(float height, float radius, int n_segments);

    void draw();

private:
    std::vector<GLfloat> m_vertices;
    std::vector<GLuint> m_indices;

    unsigned int m_vao;
    unsigned int m_vbo;
    unsigned int m_ebo;
};

#endif /* GEOMETRY_H */

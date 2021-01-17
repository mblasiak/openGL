#pragma once

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>


#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include "glad/glad.h"
#include "pyramid.h"

class Pyramid {
public:
    Pyramid();

    ~Pyramid();

    void draw();

private:
    std::vector<GLfloat> vertices = {
            0.0f, 0.5f, 0.0f, 0.3f, 0.1f, 0.6f,

            0.5f, -0.5f, 0.5f, 0.3f, 0.1f, 0.9f,
            -0.5f, -0.5f, 0.5f, 0.3f, 0.1f, 0.9f,

            0.5f, -0.5f, -0.5f, 0.9f, 0.3f, 0.3f,
            0.5f, -0.5f, 0.5f, 0.9f, 0.3f, 0.3f,

            -0.5f, -0.5f, -0.5f, 0.8f, 0.8f, 0.9f,
            -0.5f, -0.5f, 0.5f, 0.8f, 0.8f, 0.9f,

            0.5f, -0.5f, -0.5f, 0.8f, 0.2f, 0.9f,
            -0.5f, -0.5f, -0.5f, 0.8f, 0.2f, 0.9f,

            0.5f, -0.5f, -0.5f, 0.1f, 0.2f, 0.3f,
            -0.5f, -0.5f, -0.5f, 0.1f, 0.2f, 0.3f,
            0.5f, -0.5f, 0.5f, 0.1f, 0.2f, 0.3f,

            0.5f, -0.5f, 0.5f, 0.1f, 0.2f, 0.3f,
            -0.5f, -0.5f, 0.5f, 0.1f, 0.2f, 0.3f,
            -0.5f, -0.5f, -0.5f, 0.1f, 0.2f, 0.3f,
    };


    std::vector<GLushort> indices =
            {0, 2, 1,
             3, 0, 4,
             0, 5, 6,
             0, 7, 8,

             10, 9, 11,
             12, 13, 14};


    GLuint vao_;
    GLuint buffer_[2];

};



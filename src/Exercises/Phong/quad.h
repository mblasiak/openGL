#pragma once

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>


#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include "glad/glad.h"
#include "pyramid.h"

class Quad {
public:
    Quad();

    ~Quad();

    void draw();

    Quad(const Pyramid &rhs) = delete;

    Quad(Pyramid &&rhs) = delete;

    Quad &operator=(const Quad &rhs) = delete;

    void operator=(Quad &&rhs) = delete;

private:
    std::vector<GLfloat> vertices = {
            1.0f, 0.0f, 1.0f, 0.1910f, 0.500f,      0.0f, 1.0f, 0.0,
            1.0f, 0.0f, -1.0f, 0.5f, 0.8090f,       0.0f, 1.0f, 0.0,
            -1.0f, 0.0f, 1.0f, 0.5f, 0.1910f,       0.0f, 1.0f, 0.0,
            -1.0f, 0.0f, -1.0f, 0.8090f, 0.500f,    0.0f, 1.0f, 0.0,


    };


    std::vector<GLushort> indices =
            {
//             Bottom plane
             0, 1, 2,
             1, 3, 2,
            };


    GLuint vao_;
    GLuint buffer_[2];
    GLuint diffuse_texture_;


};



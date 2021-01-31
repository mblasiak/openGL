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

    Pyramid(const Pyramid &rhs) = delete;

    Pyramid(Pyramid &&rhs) = delete;

    Pyramid &operator=(const Pyramid &rhs) = delete;

    void operator=(Pyramid &&rhs) = delete;

private:
    std::vector<GLfloat> vertices = {
//           Top
            0.0f, 0.5f, 0.0f, 0.3f, 0.1f,

//          Bottom
            0.5f, -0.5f, 0.5f, 0.3f, 0.1f,
            0.5f, -0.5f, -0.5f, 0.9f, 0.3f,

            -0.5f, -0.5f, 0.5f, 0.3f, 0.1f,
            -0.5f, -0.5f, -0.5f, 0.9f, 0.3f,

//          Additional Top
            0.0f, 0.5f, 0.0f, 0.3f, 0.1f,
            0.0f, 0.5f, 0.0f, 0.3f, 0.1f,
            0.0f, 0.5f, 0.0f, 0.3f, 0.1f,


    };


    std::vector<GLushort> indices =
            {0, 1, 2,
             5, 2, 4,
             6, 4, 3,
             7, 3, 1,
//             Bottom plane
             1, 3, 2,
             2, 3, 4,
            };


    GLuint vao_;
    GLuint buffer_[2];
    GLuint diffuse_texture_;


};



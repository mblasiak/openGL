//
// Created by pbialas on 25.09.2020.
//

#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "Application/utils.h"


void SimpleShapeApplication::init() {


    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");


    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }
    auto u_modifiers_index = glGetUniformBlockIndex(program, "Modifiers");
    if (u_modifiers_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find Modifiers uniform block in program" << std::endl;
    } else { glUniformBlockBinding(program, u_modifiers_index, 0); }

    auto u_pvm_index = glGetUniformBlockIndex(program, "PVM");
    if (u_pvm_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find PVM uniform block in program" << std::endl;
    } else { glUniformBlockBinding(program, u_pvm_index, 1); }
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


    indices = {0, 2, 1,
               3, 0, 4,
               0, 5, 6,
               0, 7, 8,

               10, 9, 11,
               12, 13, 14


    };

    GLuint idx_buffer_handle;
    glGenBuffers(1, &idx_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    std::vector<float> modifier = {0.9, .8f, .9f, .99f};

    ubo_handle = GLuint(0u);
    glGenBuffers(1, &ubo_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_handle);
    glBufferData(GL_UNIFORM_BUFFER, 8 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 1 * sizeof(float), &modifier[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), 3 * sizeof(float), &modifier[1]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glm::vec3 up = {0.0, 1.0, 0.0};
    glm::vec3 eye = {+0.3, +0.7, -.9};
    glm::vec3 center = {0.0, -0.2, 0.0};

    glm::mat4 M(1.0f);
    glm::mat4 V = glm::lookAt(eye, center, up);
    double ratio = (double) this->width / (double) this->height;
    glm::mat4 P = glm::perspective(glm::radians(80.0), ratio, 0.1, 200.0);

    glm::mat4 PVM(P * V * M);
    pvm_buff_handle = GLuint(0u);
    glGenBuffers(1, &pvm_buff_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_buff_handle);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, pvm_buff_handle);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
}

void SimpleShapeApplication::frame() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glBindVertexArray(0);
}

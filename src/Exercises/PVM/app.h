//
// Created by pbialas on 05.08.2020.
//


#pragma once

#include <vector>


#include "Application/application.h"
#include "Application/utils.h"

#include "glad/glad.h"

class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
            Application(width, height, title, major, minor) {
        this->width = width;
        this->height = height;
    }

    void init() override;;

    void frame() override;

private:
    int width;
    int height;
    GLuint vao_;
    std::vector<GLushort> indices;
    GLuint ubo_handle;
    GLuint pvm_buff_handle;
};
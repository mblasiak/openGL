//
// Created by pbialas on 05.08.2020.
//


#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Application/application.h"
#include "Application/utils.h"

#include "glad/glad.h"
#include "camera.h"
#include "camera_controller.h"
#include "pyramid.h"

class SimpleShapeApplication : public xe::Application {
public:
    SimpleShapeApplication(int width, int height, std::string title, int major = 4, int minor = 1) :
            Application(width, height, title, major, minor) {
    }

    void init() override;;

    void frame() override;

    void framebuffer_resize_callback(int w, int h) override;

    void scroll_callback(double xoffset, double yoffset) override;

    void mouse_button_callback(int button, int action, int mods) override;

    void cursor_position_callback(double x, double y) override;

    Camera *camera() { return camera_; }


    void set_camera(Camera *camera) { camera_ = camera; }

    void set_controller(CameraController *controller) { controller_ = controller; }

    ~SimpleShapeApplication() {
        delete camera_;
    }

private:
    int width;
    int height;
    GLuint pvm_buff_handle;

    glm::vec3 up = {0.0, 1.0, 0.0};
    glm::vec3 eye = {+0.3, +0.7, -.9};
    glm::vec3 center = {0.0, -0.2, 0.0};

    float fov = glm::radians(80.0);
    float near = 0.1;
    float far = 200.0;

    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 PVM;

    Pyramid *pyramid;
    Camera *camera_;
    CameraController *controller_;

};
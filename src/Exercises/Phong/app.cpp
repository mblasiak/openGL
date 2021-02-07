//
// Created by pbialas on 25.09.2020.
//
#define STB_IMAGE_IMPLEMENTATION

#include "3rdParty/src/stb/stb_image.h"
#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>


#include "Application/utils.h"


void SimpleShapeApplication::init() {


    auto program = xe::create_program(std::string(PROJECT_DIR) + "/shaders/base_vs.glsl",
                                      std::string(PROJECT_DIR) + "/shaders/base_fs.glsl");

    if (!program) {
        std::cerr << "Cannot create program from " << std::string(PROJECT_DIR) + "/shaders/base_vs.glsl" << " and ";
        std::cerr << std::string(PROJECT_DIR) + "/shaders/base_fs.glsl" << " shader files" << std::endl;
    }

//    Setup PVM uniform
    auto u_pvm_index = glGetUniformBlockIndex(program, "Transformations");
    if (u_pvm_index == GL_INVALID_INDEX) {
        std::cout << "Cannot find PVM uniform block in program" << std::endl;
    } else { glUniformBlockBinding(program, u_pvm_index, 1); }

//  PVM + CAMERA Setup
//-----------------------------------------------------------------------
    set_camera(new Camera);
    set_controller(new CameraController(camera()));
    std::tie(width, height) = frame_buffer_size();
    camera_->look_at(eye, center, up);
    camera_->perspective(fov, (float) height / (float) width, near, far);
    camera_->set_aspect((float) width / (float) height);
    P = glm::mat4(camera_->projection());
    VM = glm::mat4(camera_->view() * M);
    pvm_buff_handle = GLuint(0u);
    glGenBuffers(1, &pvm_buff_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_buff_handle);
    glBufferData(GL_UNIFORM_BUFFER, (2 * 16 + 3 * 4) * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, pvm_buff_handle);
//-----------------------------------------------------------------------

    quad = std::make_shared<Quad>();


//    Clear bindings
//-----------------------------------------------------------------------
    glBindVertexArray(0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//-----------------------------------------------------------------------
//TEXTURES
    auto u_diffuse_map_location = glGetUniformLocation(program, "diffuse_map");
    if (u_diffuse_map_location == -1) {
        std::cerr << "Cannot find uniform diffuse_map\\n";
    } else {
        glUniform1ui(u_diffuse_map_location, 0);
    }


// Setup window + rendering options
//-----------------------------------------------------------------------
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
//    glEnable(GL_CULL_FACE);
//    glFrontFace(GL_CCW);
//    glCullFace(GL_BACK);
}

void SimpleShapeApplication::frame() {
//    draw objects
//-----------------------------------------------------------------------

    quad->draw();
//    Calculate camera changes
//-----------------------------------------------------------------------
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_buff_handle);
    P = glm::mat4(camera_->projection());
    VM = glm::mat4(camera_->view() * M);
//    PVM = glm::mat4(camera_->projection() * camera_->view() * M * rotation_m);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), &P[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, 16*sizeof(float), 16 * sizeof(float), &VM[0]);

//    Clear
//-----------------------------------------------------------------------
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindVertexArray(0);
//    Texture

}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera_->set_aspect((float) w / (float) h);
}

void SimpleShapeApplication::scroll_callback(double xoffset, double yoffset) {
    Application::scroll_callback(xoffset, yoffset);
    camera()->zoom(yoffset / 10.0f);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controller_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controller_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controller_->LMB_released(x, y);
    }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controller_) {
        controller_->mouse_moved(x, y);
    }
}

void SimpleShapeApplication::cleanup() {
    quad.reset();
}

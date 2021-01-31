//
// Created by pbialas on 25.09.2020.
//

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
    auto u_pvm_index = glGetUniformBlockIndex(program, "PVM");
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
    PVM = glm::mat4(camera_->projection() * camera_->view() * M);

    pvm_buff_handle = GLuint(0u);
    glGenBuffers(1, &pvm_buff_handle);
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_buff_handle);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, pvm_buff_handle);
//-----------------------------------------------------------------------

    pyramid_ = std::make_shared<Pyramid>();

//    Clear bindings
//-----------------------------------------------------------------------
    glBindVertexArray(0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//-----------------------------------------------------------------------



// Setup window + rendering options
//-----------------------------------------------------------------------
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    int w, h;
    std::tie(w, h) = frame_buffer_size();
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
//    Time
    start_ = std::chrono::steady_clock::now();
    auto rotation_angle = 4.0;

}

void SimpleShapeApplication::frame() {

    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(now - start_).count();

    auto rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / rotation_period;
    auto rotation_m = glm::rotate(glm::mat4(1.0f), rotation_angle, {0.0, 1.00, 0.0});
    orbital_rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / orbital_rotation_period;
    orbital_pos_x = ellipse_a * cos(orbital_rotation_angle);
    orbital_pos_z = ellipse_b * sin(orbital_rotation_angle);
    auto translation_m = glm::translate(glm::mat4(1.0f), glm::vec3{orbital_pos_x, 0.0, orbital_pos_z});
    auto planet_m = translation_m * rotation_m;

//-----------------------------------------------------------------------
//Planet
    glBindBuffer(GL_UNIFORM_BUFFER, pvm_buff_handle);
    PVM = glm::mat4(camera_->projection() * camera_->view() * M * planet_m);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), &PVM[0]);
    pyramid_->draw();
//Center
    PVM = glm::mat4(camera_->projection() * camera_->view() * M);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), &PVM[0]);
    pyramid_->draw();
//Moon
    auto moon_rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / moon_rotation_period;
    auto moon_rotation_m = glm::rotate(glm::mat4(1.0f), moon_rotation_angle, {0.0, 1.00, 0.0});
    moon_orbital_rotation_angle = 2.0f * glm::pi<float>() * elapsed_time / moon_rotation_angle;
    moon_orbital_pos_x = moon_ellipse_r * cos(moon_orbital_rotation_angle);
    moon_orbital_pos_x = moon_ellipse_r * sin(moon_orbital_rotation_angle);
    auto moon_tranlation_m = glm::translate(glm::mat4(1.0f), glm::vec3{moon_orbital_pos_x, 0.0, moon_orbital_pos_z});
    auto moon_scale = glm::mat4(.3f);
    moon_scale[3][3] = 1.0;
    auto moon_m = moon_tranlation_m * moon_rotation_m;

    PVM = glm::mat4(camera_->projection() * camera_->view() * M * planet_m * moon_m * moon_scale);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), &PVM[0]);
    pyramid_->draw();


//    Clear
//-----------------------------------------------------------------------
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindVertexArray(0);
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

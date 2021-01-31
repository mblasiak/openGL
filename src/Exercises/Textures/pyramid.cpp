#include <string>
#include <3rdParty/src/stb/stb_image.h>
#include <iostream>
#include "pyramid.h"

Pyramid::Pyramid() {
    glGenBuffers(1, &buffer_[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &buffer_[1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_[1]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_[1]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          reinterpret_cast<GLvoid *>(2 * sizeof(GLfloat)));

//    Texture
    int width, height, n_channels;
    auto texture_filename = std::string(PROJECT_DIR) + "/multicolor.png";
    uint8_t *data = stbi_load(texture_filename.c_str(), &width, &height, &n_channels, 0);
    if (data == nullptr) {
        std::cerr << "Error loading texture";
    }
    glGenTextures(1, &diffuse_texture_);
    glBindTexture(GL_TEXTURE_2D, diffuse_texture_);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, width, height,0, GL_RGB, GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

}

Pyramid::~Pyramid() {
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Pyramid::draw() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, reinterpret_cast<GLvoid *>(0));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,diffuse_texture_);

}
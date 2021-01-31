#version 410

layout(location=0) in  vec3 a_vertex_position;
layout(location=1) in vec2 in_texture_cords;
layout(std140) uniform PVM {
    mat4 matrix;
} pvm;
out vec2 texture_cords;

void main() {
    gl_Position.xyz=  a_vertex_position;
    gl_Position.a=1.0;
    gl_Position=pvm.matrix* gl_Position;
    texture_cords = in_texture_cords.rg;
}

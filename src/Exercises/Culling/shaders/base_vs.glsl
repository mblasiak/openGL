#version 410

layout(location=0) in  vec3 a_vertex_position;
layout(location=1) in vec3 in_vertex_color;
layout(std140) uniform PVM {
    mat4 matrix;
} pvm;
out vec3 vertex_color;

void main() {
    gl_Position.xyz=  a_vertex_position;
    gl_Position.a=1.0;
    gl_Position=pvm.matrix* gl_Position;
    vertex_color = in_vertex_color.rgb;
}

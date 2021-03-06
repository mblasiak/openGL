#version 410

layout(location=0) in  vec3 a_vertex_position;
layout(location=1) in vec2 in_texture_cords;
layout(location=2) in  vec3 a_vertex_normal;
layout(std140)  uniform Transformations {
    mat4 P;
    mat4 VM;
    mat3 N;
} tf;
out vec2 texture_cords;
out vec3 vertex_normal_in_vs;
out vec3 vertex_position_in_vs;

void main() {
    gl_Position.xyz=  a_vertex_position;
    gl_Position.a=1.0;

    texture_cords = in_texture_cords.rg;
    vertex_position_in_vs = (tf.VM*gl_Position).xyz;
    vertex_normal_in_vs = normalize(tf.N * a_vertex_normal);

    gl_Position=tf.P * tf.VM * gl_Position;

}

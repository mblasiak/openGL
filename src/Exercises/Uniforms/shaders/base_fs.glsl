#version 410

layout(std140) uniform Modifiers {
    float intensity;
    vec3 color;
} light;

layout(location=0) out vec4 vFragColor;
in vec3 vertex_color;

void main() {
    vFragColor.a = .9;
    vFragColor.rgb =light.color*light.intensity*vertex_color;
}



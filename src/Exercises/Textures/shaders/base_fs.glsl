#version 410

layout(location=0) out vec4 vFragColor;
in vec2 texture_cords;

void main() {
    vFragColor.rg = texture_cords;
}



#version 410

layout(location=0) out vec4 vFragColor;

vec3 rgb2fvec(int r, int g, int b);
vec3 greenColor = rgb2fvec(77, 99, 86);

void main() {

    vFragColor = vec4(greenColor, 1.0);
}

vec3 rgb2fvec(int r, int g, int b){
    return vec3(float(r)/255, float(g)/255, float(b)/255);
}
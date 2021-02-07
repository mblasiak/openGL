#version 410

layout(location=0) out vec4 vFragColor;
layout(std140) uniform Light {
    vec3 position_in_vs;
    vec3 color;
    vec3 a;
    vec3 ambient_color;
} light;

in vec2 texture_cords;
uniform sampler2D diffuse_map;
in vec3 vertex_normal_in_vs;
in vec3 vertex_position_in_vs;

void main() {

    vec4 diffuse_color = texture(diffuse_map, texture_cords);
    vFragColor.a = diffuse_color.a;
    vec3 normal = normalize(vertex_normal_in_vs);
    if (!gl_FrontFacing) {
        normal = -normal;
    }

    vec3 light_vector = light.position_in_vs - vertex_position_in_vs;
    float r = length(light_vector);
    light_vector/=r;
    float attenuation = 1.0/(light.a[0]+light.a[1]*r+light.a[2]*r*r);
    float light_in = max(0.0, dot(normal, light_vector))*attenuation;

    vFragColor.rgb = diffuse_color.rgb*light.ambient_color.rgb;
    vFragColor.rgb += light_in * diffuse_color.rgb * light.color;

}



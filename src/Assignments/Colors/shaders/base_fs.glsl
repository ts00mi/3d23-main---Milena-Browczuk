#version 410

layout(location=0) out vec4 vFragColor;
in vec3 Color;

void main() {
    vFragColor = vec4(Color, 1.0);
}

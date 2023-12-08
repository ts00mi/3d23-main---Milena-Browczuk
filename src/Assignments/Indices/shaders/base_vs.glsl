#version 410

layout(location=0) in vec4 a_vertex_position;
layout(location=1) in vec3 color;

out vec3 Color;

void main() {
    Color = color;
    gl_Position = a_vertex_position;
}

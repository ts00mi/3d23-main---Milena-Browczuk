#version 440

layout(location=0) in vec4 a_vertex_position;
layout(location=1) in vec3 color;

layout(location=1) out vec3 Color;

layout(std140, binding=1) uniform Transformations {
    mat4 PVM;
};

void main() {

    gl_Position =  PVM * a_vertex_position;
   
}
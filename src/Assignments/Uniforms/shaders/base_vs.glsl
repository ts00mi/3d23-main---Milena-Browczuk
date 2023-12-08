#version 440

layout(location=0) in vec4 a_vertex_position;
layout(location=1) in vec3 color;

layout(location=1) out vec3 Color;

layout(std140, binding=1) uniform Transformations {

 vec2 scale;
 vec2 translation;
 mat2 rotation;

 };

void main() {

    gl_Position.xy=a_vertex_position.xy+translation;
    gl_Position.zw = a_vertex_position.zw;
}
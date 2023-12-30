#version 440

layout(std140, binding = 0) uniform Modifier 
{
    float strength;
    vec3  color; 

} modifier;

layout(location=0) out vec4 vFragColor;
layout(location=1) in vec3 base_color;

void main() 
{
    vFragColor = vec4(base_color.xyz * modifier.color * modifier.strength, 1.0f);

}


#version 440

layout(std140, binding = 0) uniform Modifier 
{
    float strength;
    vec3  color; 

} modifier;

layout(location=0) out vec4 vFragColor;
in vec3 Color;

void main() 
{
    vFragColor = vec4(modifier.color * modifier.strength, 1.0f);
}


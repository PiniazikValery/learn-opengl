#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float lineDegree;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(vec3(model * vec4((aPos + aNormal * lineDegree), 1.0)), 1.0);
}
#version 430 core

layout(location = 0) in vec2 vertex;
layout(location = 1) in vec2 uv;

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

out vec2 vUV;

void main(void)
{
    gl_Position = vec4(vertex,0.0,1.0);
    vUV = uv;
}

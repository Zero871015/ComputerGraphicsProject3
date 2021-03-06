#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

const float pi = 3.14159;
uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;

uniform float time;
uniform int numWaves;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];
uniform vec3 EyePos;

uniform sampler2D heightmap;

out vec3 pos;
out vec3 nor;

out vec3 pos_eye;
out vec3 n_eye;
out mat4 V;

out vec2 vUV1;
out vec2 vUV2;

float wave(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + time * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int k = 0; k < numWaves; k++)
        height += wave(k, x, y);
    return height;
}


float dWavedx(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < numWaves; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, 1.0, -dy);
    return normalize(n);
}

void main(void)
{
    vUV1 = uv+time*vec2(1.0,0)*0.1;
    vUV2 = uv+time*vec2(1.0,0)*-0.1;

    //height map
    vec3 heightmapCol = vec3(texture2D(heightmap,vUV1));
    float bright = mix(heightmapCol.x,heightmapCol.y,heightmapCol.z);
    bright = bright * 4 - 2;

    V = ModelViewMatrix;
    pos = vertex;
    pos.y = waveHeight(pos.x,pos.z)+20+bright;
    nor = waveNormal(pos.x,pos.z);
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(pos, 1.0);
    nor = mat3(ModelViewMatrix) * nor;

    pos_eye = vec3(ModelViewMatrix * vec4(pos,1.0));
    //n_eye = vec3(ModelViewMatrix * vec4(0,1,0,0.0));
    n_eye = nor;
}

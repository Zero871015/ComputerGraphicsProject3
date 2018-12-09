#version 430 core

layout(location = 0) in vec3 vertex;

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

out vec3 pos;
out vec3 nor;

out vec3 pos_eye;
out vec3 n_eye;
out mat4 V;

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
    vec3 n = vec3(-dx, -dy, 1.0);
    return normalize(n);
}

void main(void)
{
    V = ModelViewMatrix;
    pos = vertex;
    pos.y = waveHeight(pos.x,pos.z)+20;
    nor = waveNormal(pos.x,pos.z);
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(pos, 1.0);
    nor = mat3(ModelViewMatrix) * nor;

    pos_eye = vec3(ModelViewMatrix * vec4(pos,1.0));
    n_eye = vec3(ModelViewMatrix * vec4(nor,1.0));
}

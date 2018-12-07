#version 430 core

in vec3 pos;
in vec3 nor;

out vec4 fColor;

uniform vec4 color_ambient = vec4(0.1,0.2,0.5,1.0);
uniform vec4 color_diffuse = vec4(0.2,0.3,0.6,1.0);
uniform vec4 color_specular = vec4(1.0,1.0,1.0,1.0);
uniform vec4 Color = vec4(0.1,0.1,0.5,1.0);
uniform float shininss = 77.0f;
uniform vec3 light_position = vec3(50.0f,32.0f,560.0f);
uniform vec3 EyeDirection = vec3(0.0,0.0,1.0);

void main()
{
vec3 light_direction = normalize(light_position - pos);
    vec3 normal = normalize(nor);
    vec3 half_vector = normalize(normalize(light_direction)+normalize(EyeDirection));
    float diffuse = max(0.0,dot(normal,light_direction));
    float specular = pow(max(0.0,dot(nor,half_vector)),shininss);
    fColor= min(Color*color_ambient,vec4(1.0))+diffuse*color_diffuse+specular*color_specular;
}

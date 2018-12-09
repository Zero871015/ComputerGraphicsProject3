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
uniform vec3 EyeDir=vec3(0.0,0.0,1.0);

uniform samplerCube skybox;
in vec3 pos_eye;
in vec3 n_eye;
in mat4 V;

void main()
{
    vec3 light_direction = normalize(light_position - pos);
    vec3 normal = normalize(nor);
    vec3 half_vector = normalize(normalize(light_direction)+normalize(EyeDir));
    float diffuse = max(0.0,dot(normal,light_direction));
    float specular = pow(max(0.0,dot(nor,half_vector)),shininss);
    fColor= min(Color*color_ambient,vec4(1.0))+diffuse*color_diffuse+specular*color_specular;

    vec3 incident_eye = normalize(pos_eye);
    vec3 r_normal = normalize(n_eye);
    vec3 reflectVec = reflect(incident_eye , r_normal);
    reflectVec = vec3(inverse(V) * vec4(reflectVec,0.0));

    vec3 refractVec = refract(incident_eye , r_normal, 0.66);
    refractVec = vec3(inverse(V) * vec4(refractVec,0.0));

    vec3 ReflectColor = vec3(textureCube(skybox,reflectVec));
    vec3 RefractColor = vec3(textureCube(skybox,refractVec));
    vec3 outputCol = mix(ReflectColor,RefractColor,0.7);

    fColor = vec4(outputCol,1.0);
}

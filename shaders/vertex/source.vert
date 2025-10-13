#version 330

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

out vec2 texCoord;
out float lightIntensity;

uniform mat4 uMVP;
uniform mat4 uModel;

void main()
{
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));

    vec3 norm = mat3(transpose(inverse(uModel))) * aNormal;

    gl_Position = uMVP * vec4(aPos, 1.0);
    texCoord = aTexCoord;
    lightIntensity = max(dot(norm, lightDir), 0.0);
}
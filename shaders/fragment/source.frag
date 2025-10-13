#version 330 core

in float lightIntensity;
in vec2 texCoord;

uniform sampler2D uTexture;
uniform float uBlend;

out vec4 FragColor;

void main()
{
    vec4 colorVal = vec4(vec3(lightIntensity), 1.0);

    vec4 texVal = texture(uTexture, texCoord);

    float smoothBlend = smoothstep(0.0, 1.0, uBlend);

    FragColor = mix(colorVal, texVal, smoothBlend);
}
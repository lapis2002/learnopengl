#version 330 core
out vec4 FragColor;

in vec3 outColor;
in vec2 outTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixValue;

void main()
{
//     FragColor = texture(texture2, outTexCoord) * vec4(outColor, 1.f);
    FragColor = mix(texture(texture1, outTexCoord), texture(texture2, outTexCoord), mixValue) * vec4(outColor, 1.f);
} 
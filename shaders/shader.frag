#version 330 core
out vec4 FragColor;

in vec2 outTexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;


void main()
{
//     FragColor = texture(texture2, outTexCoord);
    FragColor = mix(texture(texture1, outTexCoord), texture(texture2, outTexCoord), 0.2 /* 0.8 from the first texture & 0.2 from the 2nd texture */);
} 
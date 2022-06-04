#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTex;

out vec3 outColor;
out vec2 outTexCoord;

uniform mat4 transformMat;
void main()
{
    gl_Position = transformMat * vec4(aPos, 1.0);
    outColor = aCol;
    outTexCoord = aTex;
}
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 Normal;
out vec2 UV;
out vec3 pos;

void main()
{
    Normal = aNormal;
    UV = aUV;
    
    vec4 worldPos = uModel * vec4(aPos, 1.0);

    pos = worldPos.xyz;

    gl_Position = uProj * uView * worldPos;
}
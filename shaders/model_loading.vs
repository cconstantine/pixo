#version 310 es

#ifdef GL_ES
precision highp float;
#endif

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec2 texCoordsOffset;
layout (location = 4) in mat4 positionOffset;

out vec2 TexCoords;
out vec3 Normal;
out vec4 Position;

uniform mat4 view;
uniform mat4 projection;

uniform mat4 MVP;

void main()
{
    gl_Position = projection * view  * positionOffset * vec4(position, 1.0f);
    TexCoords = texCoords + texCoordsOffset;
    Normal = normal;
    Position = positionOffset*vec4(position, 1.0f);
}
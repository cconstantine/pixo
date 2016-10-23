#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 framebuf_proj;

out vec2 TexCoords;
out vec3 Position;

uniform mat4 projection;
uniform mat4 view_from;
uniform mat4 proj_from;

void main()
{
    gl_Position = projection * vec4(framebuf_proj, 1.0f);
    TexCoords =  (proj_from  * view_from * vec4(position, 1.0f)).xy / (proj_from  * view_from * vec4(position, 1.0f)).z + 0.5;
    Position = position ;
}
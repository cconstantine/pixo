#version 330 core

#ifdef GL_ES
precision highp float;
#endif

in vec2 TexCoords;
in vec3 Position;

out vec4 color;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
  color = texture(texture1, TexCoords);
}
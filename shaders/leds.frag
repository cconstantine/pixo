#version 310 es

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
  if(TexCoords != vec2(0.0f, 0.0f)) {
    color = texture(texture0, TexCoords);
  }
}
#version 330 core

in vec2 TexCoords;
in vec3 Position;

out vec4 color;

uniform sampler2D texture1;


void main()
{
  color = texture(texture1, TexCoords);
  //color = vec4(Position.x/20+0.3,Position.y/20+0.3, Position.z/20+0.3, 0.0);
  //color = vec4(1.0, 1.0, 1.0, 1.0);

}

#version 330 core

in vec2 TexCoords;
in vec3 Position;

out vec4 color;

uniform sampler2D texture1;

struct SpotLight {
  vec3 position;
  vec3 direction;
  float cutOff;
  
  float constant;
  float linear;
  float quadratic;  
}; 

struct PointLight {
  vec3 position;
  vec3 color;

  float constant;
  float linear;
  float quadratic;  
}; 

uniform SpotLight spot_light;
uniform PointLight point_light;

void main()
{
  color = texture(texture1, TexCoords*spot_light.cutOff+vec2(0.5,0.5));
  //color = vec4(Position.x/20, Position.y/20, Position.z/20, 0.0);
  //color = vec4(1.0, 1.0, 1.0, 1.0);
}
#version 330 core

in vec2 TexCoords;
in vec4 Normal;
in vec4 Position;

out vec4 color;

uniform sampler2D texture1;

struct PointLight {
  vec3 position;
  vec3 color;

  float constant;
  float linear;
  float quadratic;  
}; 

uniform PointLight point_light;

void main()
{
  float distance    = length(point_light.position - Position.xyz);
  float attenuation = 1.0f / (point_light.constant + point_light.linear * distance + point_light.quadratic * (distance * distance));

  vec3 norm = normalize(Normal.xyz);
  vec3 lightDir = normalize(point_light.position - Position.xyz);
  float diff = max(dot(norm, lightDir), 0.0);

  color = vec4((texture(texture1, TexCoords) * (attenuation*0.5 + diff)).xyz, 1.0);
	//color = vec4(vec3(gl_FragCoord.z), 1.0f);

}
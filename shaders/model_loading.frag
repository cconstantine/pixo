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

  // float distance    = length(point_light.position - Position);
  // float attenuation = 1.0f / (point_light.constant + point_light.linear * distance + point_light.quadratic * (distance * distance));   

  color = texture(texture1, TexCoords);// * attenuation;

  vec3 lightDir = normalize(spot_light.position - Position);

  // Check if lighting is inside the spotlight cone
  float theta = dot(lightDir, normalize(-spot_light.direction)); 
  if(theta > spot_light.cutOff) {
    // Attenuation
    float distance    = length(spot_light.position - Position);
    float attenuation = 1.0f / (spot_light.constant + spot_light.linear * distance + spot_light.quadratic * (distance * distance));   

    color = color*attenuation + vec4(1.0) * attenuation;
  }
}

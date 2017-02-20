#version 330 core

#ifdef GL_ES
precision highp float;
#endif

in vec2 TexCoords;
in vec3 Position;

out vec4 color;

uniform sampler2D texture0;

void main()
{
  color = texture(texture0, TexCoords);

  //float raw_depth = texture(texture1, TexCoords.xz).r;
  // if (raw_depth < 0.01f || raw_depth > 0.90f) {
  //   return;
  // }
  //float depth = raw_depth*40.0f;
  //float dist = pow(1.0f - distance(0.5, TexCoords.y), 16.0f);
  //float dist = distance(depth, TexCoords.y) < 0.02f ? 1.0f : 0.0f;  
  //color = vec4(dist );
  //color = texture(texture0, TexCoords.xz) * dist;
  //color = texture(texture0, TexCoords.xz) * TexCoords.y;// * min(distance(depth, TexCoords.y) *depth, 1.0);
}
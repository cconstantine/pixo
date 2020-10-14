out vec4 color_out;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

float checker(in float u, in float v)
{
  float checkSize = 10.0;
  float fmodResult = mod(floor(checkSize * u) + floor(checkSize * v), 2.0);
  float fin = max(sign(fmodResult), 0.0);
  return fin;
}

void main( void ) {
  color_out = vec4( vec3(checker(gl_FragCoord.x / resolution.x, gl_FragCoord.y / resolution.y)), 1.0 );

}
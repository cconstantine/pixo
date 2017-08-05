#version 330 core
out vec4 color_out;

uniform float time;
uniform vec2 mouse;
varying vec2 surfacePosition;

void main( void ) {

  float len = length(surfacePosition*100.);
  color_out = vec4( (vec3(0.5+0.5*sin(time-vec3(20.*len/pow(len,0.88))+(3.14*sin(len/pow(vec3(len),vec3(.77,.66,.45))))))), 1.0 );

}
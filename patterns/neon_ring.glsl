#version 330 core

// http://glslsandbox.com/e#32460.5

#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

out vec4 color_out;

void main( void ) {
  vec2 pos = ((gl_FragCoord.xy / resolution.xy) * 2. - 1.) * vec2(resolution.x / resolution.y, 1.0);
  
  float d = abs(0.1 + length(pos) - 1.0 * abs(sin(time - cos(time * 0.01) * 9.0 * length(pos)))) * 5.0;
  float e = abs(0.1 + length(pos) - 1.0 * abs(sin(time * 0.5 - 2.))) * 10.0;
  
  color_out += vec4(0.1/d, 0.1 / d, 0.2 / d, 1);
  color_out += vec4(0.3/e, 0.1 / e, 0.1 / e, 1);
}
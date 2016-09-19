#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
out vec4 color_out;

#define PI 3.141592653589793
#define TWOPI 6.283185307179586

float rand(vec2 co) { 
  float a = 12.9898; 
  float b = 78.233; 
  float c = 43758.5453; 
  float dt= dot(co.xy ,vec2(a,b)); 
  float sn= mod(dt,3.14); 
  return fract(sin(sn) * c);
}

vec2 rotate(float theta, vec2 v) {
  float s = sin(theta);
  float c = cos(theta);
  mat2 R = mat2(c, s, -s, c);
  return R * v;
}

void main(void) {
  vec2 pos = gl_FragCoord.xy * 2. - resolution;
  pos /= max(resolution.x, resolution.y);

  pos *= 5.;

  float l = length(pos);
  pos = rotate(-pow(l, .5), pos);

  pos *= .5;

  vec3 mask = vec3(0.0);

  const int len = 3;
  for (int i = 0; i < len; i++) {
    float theta = mod(-time, TWOPI) - PI * 2. * float(i) / float(len);
    float phi = atan(pos.y, pos.x);

    float delta = phi - theta;
    delta = mod(delta + PI, TWOPI);

    float r = smoothstep(0., TWOPI / float(len), abs(delta));

    mask += vec3(pos, 1.) * vec3(1. - r);
  }

  vec4 color = vec4(mask.xy * pos, mask.z, 1.0);

  float c = length(color.xy);
  color.rg = rotate(1. + pow(c,1.2), color.rg);
	
  color.gr = sin(color.rg * 10.);
	
	color *= step(vec4(.5), color);

  color_out = color;
}

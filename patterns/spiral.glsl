#version 310 es
#ifdef GL_ES
precision highp float;
#endif

out vec4 color_out;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
#define PI 3.14159265358979323846264338327
#define clamps(x) clamp(x,0.,1.)
//<>| I'm using an English keyboard for Swedish.
//https://en.wikipedia.org/wiki/Inversive_geometry
float atans(vec2 uv){
    return (atan(uv.x,uv.y)+PI)/(PI*2.);
}
vec3 draw(vec2 uv){
  return vec3(uv.x,uv.y,fract(length(uv*2.)+time+atans(uv)));
}
void main(void){
  vec2 uv=(gl_FragCoord.xy/resolution.xy)-.5;uv.x/=resolution.y/resolution.x;
  uv*=3.;
  vec3 final=clamps(1.-vec3(100.*abs(length(uv)-1.)));
  final+=length(uv)<1.?draw(uv):draw(uv.xy/(pow(uv.x,2.)+pow(uv.y,2.)));
  color_out=vec4(final,1.);
}
#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

// http://glslsandbox.com/e#32646.0

#define M_PI 3.1415926535897932384626433832795

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

out vec4 color_out;

void main( void ) {
  vec3 color = vec3(1.0, 1.0, 1.0);
  float shift = sin(time * 1.0);
  float width = 50.0;
  float x = gl_FragCoord.x - resolution.x/2.0;
  float y = gl_FragCoord.y - resolution.y/2.0;
  //float angle = atan(mouse.y, mouse.x);
  //x = x*cos(angle)-y*sin(angle);
  //y = 
  /*if ((x > -1.5 && x < 1.5) || (y > -1.5 && y < 1.5)) {
    color = vec3(0.0, 1.0, 1.0);  
  }
  else*/ {
    float s = y < 0.0 ? 1.0 : -1.0;
    float angle = M_PI/4.0;
    
    float xp = x*cos(angle*s) - y*sin(angle*s) - time*100.0;
    
    if (mod(xp, width) - width/2.0 > 0.0) {
      color = vec3(0.0, 0.0, 0.0);
    }
  }
  
  
  color_out = vec4( color, 1.0 );

}
#ifdef GL_ES
precision mediump float;
#endif

// http://glslsandbox.com/e#27440.0

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform sampler2D backbuffer;

vec3 Hue( float hue )
{
  vec3 rgb = fract(hue + vec3(0.0,2.0/3.0,1.0/3.0));

  rgb = abs(rgb*2.0-1.0);
    
  return clamp(rgb*3.0-1.0,0.0,1.0);
}

vec4 toRGB( vec4 hsv )
{
  return vec4( ((Hue(hsv.x)-1.0)*hsv.y+1.0) * hsv.z, hsv.a);
}

void main( void ) {

  vec2 texcoord = vec2(gl_FragCoord.xy/resolution);
  vec2 coord = ( gl_FragCoord.xy*2.0 - resolution ) / resolution.y;

  vec4 prev =texture2D(backbuffer,texcoord *vec2(0.5));
  vec4 prev1 =texture2D(backbuffer,texcoord*vec2(0.1));
  
  float x = fract(coord.x*2.0+sin(time+coord.y));
  float y = fract(coord.y*2.0+sin(time+coord.x));
  
  y = pow(abs(y-0.5), 2.0)*2.0;
  x = pow(abs(x-0.5), 2.0)*2.0;
  
  float h = texcoord.x * texcoord.y;
  vec4 color = toRGB(vec4(h,0.5,1.0,1.0));
  
  vec4 s1 =vec4(y+x)*color + (prev*0.2) + (prev1*0.2);
  
  gl_FragColor = s1;

}
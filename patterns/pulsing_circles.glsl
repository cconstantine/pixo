out vec4 color_out;


// http://glslsandbox.com/e#31456.1

// // #extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;

in vec2 surfacePosition;
in vec2 surfaceSize;

void main( void ) {
  #define t (time * 2.0)
  vec2 position = surfacePosition;
  float l = length(position);
  float dist = l * 7.564 - sin(t)*.5;
  dist = pow(dist, 1.2);
  vec2 normal = position + normalize(position) * cos(dist) / 2.0*sin(time);

  float color = (mod(normal.x + 0.09, 0.2) < 0.18) && (mod(normal.y + 0.09, 0.2) < 0.18) ? 0.5 : 0.1;
  
  color = sin(color-dist);

  color_out = vec4(color*vec3(cos(time+color+l),cos(color+time*2.5/l),cos(color+time*5.0)), 1.0 );

}
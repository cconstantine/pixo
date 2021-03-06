// Plasam by @Dorald
// http://glslsandbox.com/e#31579.0

// #extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
uniform vec2 center;
out vec4 color_out;

void main(void)
{
   float x = gl_FragCoord.x - (center.x - resolution.x / 2.0);
   float y = gl_FragCoord.y - (center.y - resolution.y / 2.0);
   float mov0 = x+y+cos(sin(time)*2.)*100.+sin(x/100.)*1000.;
   float mov1 = y / resolution.y / 0.2 + time;
   float mov2 = x / resolution.x / 0.2;
   float c1 = abs(sin(mov1+time)/2.+mov2/2.-mov1-mov2+time);
   float c2 = abs(sin(c1+sin(mov0/1000.+time)+sin(y/40.+time)+sin((x+y)/100.)*3.));
   float c3 = abs(sin(c2+cos(mov1+mov2+c2)+cos(mov2)+sin(x/1000.)));
   color_out = vec4( c1,c2,c3,1.0);
}
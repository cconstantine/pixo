#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

// http://glslsandbox.com/e#29536.0

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
out vec4 color_out;


vec2 R = resolution;
vec2 Offset;
vec2 Scale=vec2(0.005,0.005);
float Saturation = 0.4; // 0 - 1;


vec3 lungth(vec2 x,vec3 c){//FIFY
       return vec3(length(x+c.rg),length(x+c.gb),length(x+c.br));
}

void main( void ) {
    Offset = mouse.xy;
    vec2 x = gl_FragCoord.xy;
    vec4 c=vec4(0,0,0,0);
    x=x*Scale*R/R.x+Offset;
    x+=sin(x.yx*sqrt(vec2(13,9)))/5.4;
    c.rgb=lungth(sin(x*sqrt(vec2(33,43))),vec3(3,1,9)*Saturation);
    x+=sin(x.yx*sqrt(vec2(17,19)))/5.1;
    c.rgb=1.5*lungth(sin(time+x*sqrt(vec2(13.7,47.7))),c.rgb/9.2);
    x+=sin(x.yx*sqrt(vec2(89,51)))/2.2;
    c.rgb=lungth(sin(x*sqrt(vec2(11.1,1.1))),c.rgb/3.1);
    c=.4+.4*sin(c*8.);
    c.a=1.;
    color_out = c;
}
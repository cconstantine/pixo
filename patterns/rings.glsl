#version 330 core
out vec4 color_out;


#ifdef GL_ES
precision mediump float;
#endif

// http://glslsandbox.com/e#36175.2

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

struct circ {
	vec2 pos;
	vec3 col;
};

vec3 color = vec3(0.0);
float speed = 0.5;
float scale = distance(tan(time*speed),atan(time*speed))+2.0;
//float scale = -distance(tan(time*speed),-tan(time*speed))-2.0;
float size = 0.4;
float rad = 5.0;
								//position
circ c1 = circ(vec2(0,-1.0)/scale,vec3(0.4,0.1,0.1));		//0°
circ c2 = circ(vec2(-0.8660,-0.5)/scale,vec3(0.3,0.1,0.2));	//60°
circ c3 = circ(vec2(-0.8660,0.5)/scale,vec3(0.2,0.1,0.3));	//...
circ c4 = circ(vec2(0.0,1.0)/scale,vec3(0.1,0.1,0.4));
circ c5 = circ(vec2(0.8660,0.5)/scale,vec3(0.2,0.1,0.3));
circ c6 = circ(vec2(0.8660,-0.5)/scale,vec3(0.3,0.1,0.2));
//circ c7 = circ(vec2(0,0.0)/scale,vec3(0.1,0.1,0.1));	 	//center
	
float circle(vec2 uv, circ circl){
	float cir = ((((circl.pos.y-uv.y)*(circl.pos.y-uv.y))+((circl.pos.x-uv.x)*(circl.pos.x-uv.x)))*20.0);
	float c = distance(rad,cir)/size;
	return c;
}
void main( void ) {
	vec2 uv = gl_FragCoord.xy / resolution.xy * 2.0 - 1.0;
	uv.x *= resolution.x/resolution.y;
	
	float c11 = circle(uv, c1);
	float c22 = circle(uv, c2);
	float c33 = circle(uv, c3);
	float c44 = circle(uv, c4);
	float c55 = circle(uv, c5);
	float c66 = circle(uv, c6);
	//float c77 = circle(uv, c7);
	
	color += c1.col/c11;
	color += c2.col/c22;
	color += c3.col/c33;
	color += c4.col/c44;
	color += c5.col/c55;
	color += c6.col/c66;
	//color += c7.col/c77;
	
	color_out = vec4(color, 1.0 );

}
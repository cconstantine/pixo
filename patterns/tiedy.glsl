#ifdef GL_ES
precision mediump float;
#endif

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

// bpt.modified.2016 
// started with http://glslsandbox.com/e#7886.9

float sdEpicyloidLike(vec2 center, float radius, vec2 position, float n)
{
	float l = distance(position, center);
	float r1 = radius+0.2/(1.0+abs(sin(n*atan(position.y-center.y, position.x-center.x))));
	return clamp(abs(sin(((l * r1)) + 0.3)),-10.0,10.0);
}


float test(vec2 center, float radius, vec2 position)
{
	return sdEpicyloidLike( center, radius, position, floor(abs(10.*sin(time*0.3)))*2.+0. );
}

void main(void)
{
	vec2 position = 4.0*(gl_FragCoord.xy - 0.5 * resolution) / resolution.y;
	
	float enabler = 1.0;
	
	float r = test(vec2(sin(time*1.01), 0.5*cos(time*0.98)), 1.5-sin(time*.9), position);
	float g = enabler*test(vec2(cos(time*0.94), sin(time*0.97)), 1.5-sin(time*.8), position);
	float b = enabler*test(vec2(sin(time*0.93), sin(time*0.99)), 1.5-sin(time*.7), position);
	
	gl_FragColor = vec4(r, g, b, 1.0);
}

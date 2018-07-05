
out vec4 color_out;
uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;
in vec2 surfacePosition;

vec3 getColor(float phase);

#define stripes 1.
#define k 4.
#define sx surfacePosition.x
#define sy surfacePosition.y

float PI = 4.*atan(1.);

void main(void) {
	vec3 c = getColor(time*0.7*PI);
	color_out = vec4(c, 1.0);

}

vec3 getColor(float phase) {
	float x = sx*.3;
	float y = sy*.3;
	float theta = atan(y, x) ;
	float r = log((x*x + y*y));//(mouse.x*2.);
	float c = .0;
	float tt;
	for (float t=0.; t<k; t++) {
		float tt = t * PI;
		c += sin(((k-t)*theta*cos(tt/*+theta*/) + t*r*sin(k-t)) * stripes - phase);
		phase*=.7;
	}
	//c = (c+k) / (k*2.);
	//c = c > 0.5 ? 1. : 0.;
	c*=0.3;
	return vec3(abs(c), c*c, -c);
	//return vec3(c, c, c);

}
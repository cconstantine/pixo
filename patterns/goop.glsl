#version 330 core
out vec4 color_out;


#ifdef GL_ES
precision mediump float;
#endif

// #extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

//----------------------------------------------------------
// metaballss by pailhead in 2014-Jun-2
// original:  https://www.shadertoy.com/view/4sXXWM
// small modifications & antialiasing by I.G.P. 2016-09-24
// This must be nice in 3d too...
//----------------------------------------------------------
// http://glslsandbox.com/e#35480.0

#define COUNT 55

#define SPEED 0.1

#define SIZE 44.

//----------------------------------------------------------
float rn(float x)
{
    return  fract(sin(x*.4686)*3718.927);
}

//----------------------------------------------------------
void mainImage( out vec4 fragColor, in vec2 fragCoord ) 
{
	float ts = time * SPEED;   // time scaled
	
	vec3 COLOR_MASKS[16];  // blob colors
	COLOR_MASKS[0] = vec3(1.00, 0.50, 0.00 );
  	COLOR_MASKS[1] = vec3(0.53, 0.85, 0.25 );
  	COLOR_MASKS[2] = vec3(1.00, 0.56, 0.15 );
  	COLOR_MASKS[3] = vec3(1.00, 0.00, 0.30 );
  	COLOR_MASKS[4] = vec3(0.05, 0.55, 0.30 );
	COLOR_MASKS[5] = vec3(0.50, 1.00, 0.40 );
  	COLOR_MASKS[6] = vec3(1.00, 0.15, 1.00 );
  	COLOR_MASKS[7] = vec3(0.20, 0.30, 0.50 );
  	COLOR_MASKS[8] = vec3(0.35, 1.00, 0.50 );
  	COLOR_MASKS[9] = vec3(0.70, 0.60, 0.50 );
  	COLOR_MASKS[10]= vec3(0.34, 1.00, 0.50 );
  	COLOR_MASKS[11]= vec3(0.20, 0.50, 0.50 );
  	COLOR_MASKS[12]= vec3(0.60, 0.10, 0.65 );
  	COLOR_MASKS[13]= vec3(0.40, 0.40, 0.85 );
  	COLOR_MASKS[14]= vec3(1.00, 0.30, 0.35 );
  	COLOR_MASKS[15]= vec3(1.00, 0.00, 0.50 );
	
	//screen space
	float aspect = resolution.x / resolution.y;
	vec2 fragPN = (fragCoord / resolution.xy) * 2.0 - 1.0;   //-1 .. 1
	fragPN.x *= aspect;
	float vH = 10.0;   // camera disance - fov
	vec3 vD = normalize(vec3(-fragPN,vH));   // view dir
	//vec3 lD = normalize(vec3(cos(time),.750,sin(time)));   // light dir	
	
	vec3 lD = normalize(vec3(mouse.xy-vec2(0.5), 0.5));   // light dir	
	
	vec3 ld = normalize(vec3(.7, .750,2.));
	
	vec2 mbPos[COUNT];  // blob position
	vec3 nn = vec3(.0);
	vec3 cc = vec3(.0);

	for(int i=0; i<COUNT; i++)
	{
		vec3 bC = COLOR_MASKS[i-(i/15)*15]; // blob color
		float rn1 = rn(float(i+47));
		float rn2 = rn(float(i-21));
		float rn3 = rn(float(i-52));
		
		if (i==0)   // set blob[0] to mouse position
		     mbPos[i] = 1.25*(mouse.xy*2. -1.) * vec2(aspect,1.);   // mouse position
		else mbPos[i] = vec2(sin(rn1*6.14+ts * rn2)*aspect,
			        cos(rn2*6.14+ts * rn3));  // calc position
		
		mbPos[i] = fragPN - mbPos[i]*.8;
		float rr = cos(rn3*6.28 +ts * rn1)*.2 +.5;
		mbPos[i] *= rr * (1000. / SIZE);   // blob coord
		float bL = length( mbPos[i] );     // blob length
		float bH = exp(-bL*2.15678);
		vec3 bN = vec3(mbPos[i] * .3 *bH, bH-.01);
		bC *= bH;
		nn += vec3(mbPos[i]*.5*bH,bH);
		cc += bC;
	}
	
	vec3 n = normalize( vec3(nn.x, nn.y, nn.z-0.1) );
	float aB = smoothstep(0.0, 0.5, n.z);  // antialiasing edge !!!
	cc /= nn.z;
	float ndl = 0.3 + 0.7 * dot(n,lD);   //wrap
	vec3 h = normalize(vD+lD);
	float ndh = 0.5 + 0.5 * dot(n,h);
	ndh = pow(ndh,1.+2000.0 *(mouse.x))*(0.2 + .65*mouse.x);  // light reflection
	vec3 fc = cc*ndl +ndh;
	
	float frs = dot(n,vD);
	frs = 1.0-clamp(frs,.0,1.);
	frs = pow(frs,100.0);
	frs = frs*.4 +.121;
	fc += frs;
	fragColor = vec4( fc*aB, 1.);
}

void main( void ) 
{
	mainImage( color_out, gl_FragCoord.xy );
}
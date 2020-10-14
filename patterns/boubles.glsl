// http://glslsandbox.com/e#61915.0
/*
 * Original shader from: https://www.shadertoy.com/view/3lKSWc
 */

out vec4 color_out;

// glslsandbox uniforms
uniform float time;
uniform vec2 resolution;

// shadertoy emulation
#define iTime time
#define iResolution resolution

// --------[ Original ShaderToy begins here ]---------- //
float map(vec3 p)
{
    vec3 q = fract(p) * 2.0 - 1.0;
    
  return length(q) - 0.25;
}

float trace(vec3 o, vec3 r)
{
    float t = 0.0;
    for (int i = 0; i < 32; ++i)
    {
        vec3 p= o + r * t;
        float d = map(p);
        t += d * 0.5;
    }
    return t;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec2 normalUV = uv;
    
    uv = uv * 2.0 - 1.0;
    uv.x *= iResolution.x / iResolution.y;
    
    vec3 r = normalize(vec3(uv, 2.0));
    
    float the = iTime * 0.25;
    mat2 rotate = mat2(cos(the), -sin(the), sin(the), cos(the));
    r.zx *= rotate;
    r.xy *= rotate;
    
    vec3 o = vec3(0.0, iTime, iTime);
    
    float t = trace(o, r);
    
    float fog = 1.0 / (1.0 + t * t * 0.1);
    
    vec3 fc = vec3(pow(fog, 1.25), pow(fog, 1.5), pow(fog, 0.75));
    fc *= vec3(uv.x+2.0, uv.y+2.0, uv.x+uv.y);
    fc /= (1.0 + distance(normalUV.x, 0.5));
    fc /= (1.0 + distance(normalUV.y, 0.5));
    fragColor = vec4(fc, 1.0);
}
// --------[ Original ShaderToy ends here ]---------- //

void main(void)
{
    mainImage(color_out, gl_FragCoord.xy);
}
out vec4 color_out;

// woah circle
// thanks http://glslsandbox.com/e#41504.0
// http://glslsandbox.com/e#68183.0

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void main()
{
    vec2 r = resolution,
    o = gl_FragCoord.xy - r/2.;
    o = vec2(length(o) / r.y - .3, atan(o.y,o.x));    
    vec4 s = 0.08*cos(1.5*vec4(0,1,2,3) + time + o.y + sin(o.x) * cos(time)),
    e = s.yzwx, 
    f = min(o.x-s,e-o.x);
    color_out = dot(clamp(f*r.y,0.,1.), 50.*(s-e)) * (s-.1) /*- f*/;
}

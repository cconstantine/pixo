out vec4 color_out;
// http://glslsandbox.com/e#39265.0

uniform float time;
uniform vec2 resolution;
// Leadlight by @hintz 2013-05-02
// modified by Harry   2014-01-05
void main()
{
  /// integers values align perfectly
  float numSides = 9.;
  
  float brightness = 5.;// + abs(cos(time/5.) * 3.);
  float size = 7. + sin(time/20.) * 50.;
  
  vec2 position = (gl_FragCoord.xy - 0.5*resolution) / resolution.xx ;
  float r = length(position);
  float a = atan(position.y, position.x);
  float t = time + size/(r+1.0);  
  float light = brightness*abs(0.05*(sin(t)+sin(time+a*numSides) ) );
  vec3 color = vec3(-sin(r*5.0-a-time+sin(r+t)), sin(r*3.0+a-time+sin(r+t)), cos(r+a*2.0+a+time)-sin(r+t));
  color_out = vec4((normalize(color)+1.7) * light , 1.0);
}
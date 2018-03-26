out vec4 color_out;

in vec2 surfacePosition;
uniform float time;


const float color_intensity = 0.9;
const float Pi = 3.14159;

void main()
{
  vec2 p=(2.*surfacePosition);
  for(int i=1;i<6;i++)
  {
      vec2 newp=p;
  float ii = float(i);  
      newp.x+=0.55/ii*sin(ii*Pi*p.y+time*.01+cos((time/(10.*ii))*ii));
      newp.y+=0.55/ii*cos(ii*Pi*p.x+time*.01+sin((time/(10.*ii))*ii));
      p=newp;
  }
  vec3 col=vec3(cos(p.x+p.y+3.+time)*.5+.5,sin(p.x+p.y+6.+time)*.5+.5,(sin(p.x+p.y+9.+time)+cos(p.x+p.y+12.+time))*.25+.5);
  color_out=vec4(col*col, 0.5);
}

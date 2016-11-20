#version 310 es

#ifdef GL_ES
precision highp float;
#endif


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 surfacePosAttrib;
out vec2 surfacePosition;
void main() {
  surfacePosition = position.xy;
  gl_Position = vec4( position, 1.0 );
}
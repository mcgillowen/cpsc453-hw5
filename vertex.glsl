#version 410

layout(location = 0) in vec2 position;

uniform mat4x4 S;
uniform mat4x4 T;

void main() {
  gl_Position = vec4(position, 0.0, 1.0);
}

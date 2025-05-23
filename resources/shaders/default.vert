#version 300 es

precision mediump float;
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
// uniform mat4 projection;

out vec3 ec_pos;

void main() {
    gl_Position = model * view * vec4(aPos, 1.0);
    ec_pos = gl_Position.xyz;
}

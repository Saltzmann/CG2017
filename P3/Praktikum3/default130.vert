#version 130

// default130.vert : a simple vertex shader

uniform mat4 matrix;
attribute vec4 vert;
attribute vec4 color;
varying vec4 col;

void main() {
    col = color;
    gl_Position = matrix * vert;
}

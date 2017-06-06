#version 330

#extension GL_ARB_separate_shader_objects : enable
// norm330.frag : a simple normals shader
//
// notes:
// GL_ARB_separate_shader_objects is not needed for version >= 410

uniform sampler2D texture; //wird genauso wie texC atm nicht benutzt
layout(location = 0)in vec4 col;
layout(location = 1)in vec4 texC;
layout(location = 0)out vec4 fragColor; // must be at 0

void main() {
    gl_FragColor = col;
}

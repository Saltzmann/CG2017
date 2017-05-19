#version 330

//#extension GL_ARB_separate_shader_objects : enable
// default330. frag : a simple fragment shader
//
// notes:
// GL_ARB_separate_shader_objects is not needed for version >= 410

layout(location = 0)in vec4 col;
layout(location = 0)out vec4 fragColor; // must be at 0

void main() {
    fragColor = col;
}

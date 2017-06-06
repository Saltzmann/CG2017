#version 330

#extension GL_ARB_explicit_uniform_location : enable

//#extension GL_ARB_separate_shader_objects : enable

// norm330.vert : a simple normals shader
//
// notes:
// GL_ARB_explicit_uniform_location is not needed for version >= 430 -> auch hier brauche ich das trotz OpenGL 4.4
// GL_ARB_separate_shader_objects is not needed for version >= 410

layout(location = 0)uniform mat4 projectionMatrix;
layout(location = 1)uniform mat4 viewMatrix;
layout(location = 2)uniform mat4 modelMatrix;
layout(location = 0)in vec4 vert;
layout(location = 1)in vec4 norm;
layout(location = 2)in vec4 texCoord; //wird genauso wie texC atm nicht benutzt
layout(location = 0)out vec4 col;
layout(location = 1)out vec4 texC;

void main() {
    col = norm;
    mat4 matrix = projectionMatrix * viewMatrix * modelMatrix;
    gl_Position = matrix * vert;
    texC = texCoord;
}

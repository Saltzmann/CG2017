#version 330

#extension GL_ARB_explicit_uniform_location : enable
//#extension GL_ARB_separate_shader_objects : enable

// default330.vert : a simple vertex shader
//
// notes:
// GL_ARB_explicit_uniform_location is not needed for version >= 430
// GL_ARB_separate_shader_objects is not needed for version >= 410

layout(location = 0)uniform mat4 projectionMatrix;
layout(location = 1)uniform mat4 viewMatrix;
layout(location = 2)uniform mat4 modelMatrix;
layout(location = 3)uniform mat3 normalMatrix;
layout(location = 0)in vec4 vertexPosition;
layout(location = 1)in vec4 vertexNormal;
layout(location = 2)in vec4 texCoord;
layout(location = 1)out vec4 texC;
layout(location = 2)out vec3 normal;
layout(location = 3)out vec3 position;

void main() {
    mat4 matrix = projectionMatrix * viewMatrix * modelMatrix;
    mat4 modelViewMatrix = viewMatrix * modelMatrix;
    texC = texCoord;
    normal = normalize(normalMatrix * vertexNormal.xyz); //normale im world space ausrichten
    position =  vec3(modelViewMatrix * vertexPosition); //vertex im world space ausrichten
    gl_Position = matrix * vertexPosition; // vertex im screen space positionieren
}

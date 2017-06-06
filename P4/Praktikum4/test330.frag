#version 330

//#extension GL_ARB_separate_shader_objects : enable
// test330.frag : a horrible fragment shader
//
// notes:
// GL_ARB_separate_shader_objects is not needed for version >= 410

uniform sampler2D diffuseMap;
uniform sampler2D distortionMap;
layout(location = 1)in vec4 texC;
layout(location = 2)uniform float time;
layout(location = 0)out vec4 fragColor; // must be at 0

void main() {
    //hoch illegaler Testshader mit if-else, der das Vorhandensein,
    //der zwei Texturen sowie der Zeitkomponente überprüft
    if(texC.x >= 0.5) {
        gl_FragColor = texture2D(diffuseMap, vec2(texC.xy));
    }
    else {
        gl_FragColor = texture2D(distortionMap, vec2(texC.xy) *  time);
    }
}

#version 330

//#extension GL_ARB_separate_shader_objects : enable
// default330. frag : a simple fragment shader
//
// notes:
// GL_ARB_separate_shader_objects is not needed for version >= 410

uniform sampler2D diffuseMap;
uniform sampler2D distortionMap;
//layout(location = 0)in vec4 col;
layout(location = 1)in vec4 texC;
//layout(location = 2)in float time;
layout(location = 0)out vec4 fragColor; // must be at 0

void main() {
    // Lese die Farbe aus der Textur aus
    // texture2D erwartet einen vec2, daher lesen wir die ersten
    // zwei Komponenten aus und packen diese in einen vec2
    if(texC.x >= 0.5) {
        gl_FragColor = texture2D(diffuseMap, vec2(texC.xy));
    }
    else {
        gl_FragColor = texture2D(distortionMap, vec2(texC.xy));
    }
}

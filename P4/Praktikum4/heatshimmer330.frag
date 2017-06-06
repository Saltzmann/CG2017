#version 330

#extension GL_ARB_explicit_uniform_location : enable
// heatshimmer330.frag
//
//extension brauche ich trotz OpenGL 4.4

uniform sampler2D diffuseMap;
uniform sampler2D distortionMap;
uniform float timeY;
uniform float timeX;
layout(location = 1)in vec4 diffuseMapCoords;
layout(location = 0)out vec4 fragColor; // must be at 0

void main() {
    vec4 distortionMapCoords = diffuseMapCoords; //Wert kopieren
    distortionMapCoords.y -= timeY * 0.2; //y coord zeitabhängig verschieben
    distortionMapCoords.x -= timeX * 0.2;
    //aus distortionMap an verschobener Stelle auslesen (= Farbe an der Stelle)
    vec4 distortionValue = texture2D(distortionMap, vec2(distortionMapCoords.xy));
    //r und g wert bilden den Werte shift?
    vec2 distortionOffset = vec2(distortionValue.xy);
    distortionOffset -= (0.5, 0.5); //Rangeänderung 1/2
    distortionOffset *= 2; //Rangeänderung 2/2
    distortionOffset *= 0.075; //Modifikatior
    //distortionOffset *= (1 - diffuseMapCoords.y);
    vec2 distortedCoord = vec2(diffuseMapCoords.xy) + distortionOffset;
    vec4 color1 = texture2D(diffuseMap, vec2(diffuseMapCoords.xy));
    vec4 color2 = texture2D(diffuseMap, vec2(distortedCoord));
    gl_FragColor = color1 * color2;
}

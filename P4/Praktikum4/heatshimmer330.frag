#version 330

//#extension GL_ARB_separate_shader_objects : enable
// default330. frag : a simple fragment shader
//
// notes:
// GL_ARB_separate_shader_objects is not needed for version >= 410
uniform sampler2D diffuseMap;
uniform sampler2D distortionMap;
uniform float timeY;
uniform float timeX;
//layout(location = 0)in vec4 col;
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

// Eingabevariablen
// diffuseMap := gewöhnliche Textur des Objekts
// distortionMap := Verzerrungstextur
// diffuseMapCoord := Texturkoordinaten der gewöhnlichen Textur
// t := zeitabhängige Variable
// Koordinaten zum Bestimmen der auszulesenden Stelle in der Verzerrungstextur
// distortionMapCoord := diffuseMapCoord

// Verschiebe die Verzerrungskoordinaten abhängig von der Zeit nach oben
// Die Konstante beeinflusst die Geschwindigkeit
// distortionMapCoord.y -= t * 0.2
// Lese Werte aus der Verzerrungstextur aus
// distortionValue := (Farb-)werte in distortionMap an Stelle distortionMapCoord
// Verwende die ersten zwei Komponenten als Verschiebung
// distortionOffset := distortionValue.xy
// Von OpenGL normalisierte Werte liegen im Intervall [0,1]
// Wir wollen eine Verschiebung in beide Richtungen, also im Intervall [ - 1,1]
// distortionOffset -= (0.5, 0.5)
// distortionOffset *= 2
// Skaliere die Verschiebung und bestimme damit die Stärke der Verzerrung
// distortionOffset *= 0.05
// Mildere den Effekt im oberen Bereich der Textur
// (optional) distortionOffset *= (1 - diffuseMapCoord.y);
// Berechne die verschobenen Texturkoordinaten
// distortedCoord := diffuseMapCoord + distortionOffset
// Lese Farben aus Texturen aus
// color1 := Farbe in diffuseMap an Stelle diffuseMapCoord
// color2 := Farbe in diffuseMap an Stelle distortedCoord
// Multiplikatives Überblenden der zwei Farben (blend mode multiply)
// Die Multiplikation zwischen zwei Vektoren mit * erfolgt komponentenweise,
// also (a, b, c, d) * (e, f , g, h) = (a*e, b*f, c*g, d*h)
// outputColor := color1 * color2

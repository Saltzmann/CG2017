#version 330

#extension GL_ARB_explicit_uniform_location : enable
// phong330.frag : a simple phong shader
//
//extension brauche ich trotz OpenGL 4.4

uniform sampler2D texture;
layout(location = 1)in vec4 texC;
layout(location = 2)in vec3 normal;
layout(location = 3)in vec3 position;
layout(location = 4)uniform vec4 lightPosition; //light position in world coordinates
layout(location = 5)uniform vec3 lightIntensity; //light 'color'
layout(location = 6)uniform vec3 kd; //diffuse
layout(location = 7)uniform vec3 ka; //ambient
layout(location = 8)uniform vec3 ks; //specular
layout(location = 9)uniform float shininess;//controls specular effect
layout(location = 10)uniform float killswitch;
layout(location = 11)uniform float skybox;
layout(location = 12)uniform float maxDistance;
layout(location = 13)uniform float linMod;
layout(location = 14)uniform float expMod;
layout(location = 0)out vec4 fragColor; // must be at 0

void main() {
    vec3 n = normalize(normal * skybox); //auf Länge 1 - n = normale an der aktuellen VertexPos
    vec3 s = normalize(lightPosition.xyz - position); // normalisierter Richtungsvector
    //von aktueller position zu Lichtquelle
    float lightIntensityPerDistance = distance(lightPosition.xyz, position) / maxDistance; //maß für distanz in der Galaxie
    //max 1 wenn so weit weg wie skybox ansonsten kleiner
    lightIntensityPerDistance = 1/lightIntensityPerDistance; // zu Faktor gemacht
    lightIntensityPerDistance *= linMod; //Faktor linear ändern
    lightIntensityPerDistance = pow(lightIntensityPerDistance, expMod); //Faktor exponentiell ändern
    vec3 v = normalize((-position)); //normalisierter Vektor der vom ursprung weg von Position zeigt?
    vec3 r = reflect (-s, n); //Ausfallwinkel von lichtstrahl der von der Lichtquelle kommt
    // und an der aktuellen Position von der Fläche abgeleitet wird
    vec3 textureColor = texture2D(texture, texC.xy).xyz;
    float colorMod = max(dot(s,n), 0.0);
    kd; //Erwähnung, weil er sich sonst beschweren kann, dass es übergeben aber nicht genutzt wird
    vec3 outputColor = lightIntensity *
            ( ka + //umgebungsfarbe
              textureColor *  //"Farbe" des objekts
              colorMod *
              lightIntensityPerDistance + //wenn parallel s*n sonst immer kleiner bis 0 bei 90° => beleuchtungshelligkeit
              ks * pow(max(dot(r, v), 0.0), shininess) * killswitch); //hightlight beleuchtung abhängig vom reflexionswinkel

    gl_FragColor = vec4(outputColor, 1.0f);
}

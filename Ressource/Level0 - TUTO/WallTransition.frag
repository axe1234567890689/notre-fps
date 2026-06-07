#version 330 core

in vec3 Normal;
in vec2 UV;
in vec3 pos;

out vec4 FragColor;

struct Ligth {
    vec3 pos;
    vec4 color;
    float power;
};

uniform struct Ligth ligths[10];

uniform int nbLigth;

uniform sampler2D texTuto;
uniform sampler2D texLevel2;

uniform vec2 transiPos;
uniform float transiDist;

void main()
{
    vec3 dir;
    float diff, dist;
    vec4 color = (nbLigth == 0) ? vec4(1., 1., 1., 1.) : vec4(0., 0., 0., 1.);

    for (int i = 0; i < nbLigth; i++) {
        dir = ligths[i].pos - pos;
        dist = length(dir);
        dir /= dist;
        diff = max(dot(normalize(Normal), normalize(dir)), 0.) / dist * ligths[i].power;
        diff = clamp(diff, 0., 1.);
        diff *= 0.75;
        diff += 0.25;

        color += ligths[i].color * diff;
    }

    vec2 dir2 = transiPos.xy - pos.xz;
    dist = length(dir2);
    float transi = dist - transiDist;// + cos(atan(dir2.y, dir2.x) * 3.) / 3.;
    if (transi > 0.7)
    transi = 1.;
    transi = clamp(transi, 0., 1.);

    FragColor = vec4((color.rgb * (texture2D(texTuto, UV).rgb * transi + texture2D(texLevel2, UV).rgb * (1. - transi))), 1.);
}

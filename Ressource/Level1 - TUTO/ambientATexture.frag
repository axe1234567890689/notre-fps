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

uniform sampler2D tex;

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
        clamp(0., 1., diff);

        color += ligths[i].color * diff;
    }

    FragColor = vec4((color * texture2D(tex, UV)).rgb, 1.);
}

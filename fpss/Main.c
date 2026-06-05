#include "Object3D.h"
#include "Entity.h"
#include "Player.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define RENDER_DIST 5

int main(void)
{
    sfContextSettings settings = {
        .depthBits = 24,
        .stencilBits = 8,
        .antialiasingLevel = 4,
        .majorVersion = 3,
        .minorVersion = 3
    };

    sfRenderWindow* window = sfRenderWindow_create((sfVideoMode) { 1920, 1080, 32 }, "CSFML 3D", sfFullscreen, &settings );

    if (!gladLoadGL())
    {
        printf("Erreur GLAD\n");
        return 1;
    }

    if (!window)
        return 1;
    
    sfRenderWindow_setMouseCursorVisible(window, sfTrue);

    object3D_init();

    sfClock* clock = sfClock_create();

    float cubeAngle = 0.f;
    float dt;
    sfEvent event;

    sfShader* shaderAT = sfShader_createFromFile("..\\Ressource\\Level1 - TUTO\\ambient.vert", NULL, "..\\Ressource\\Level1 - TUTO\\ambientATexture.frag");
    GLuint shaderIDAT = sfShader_getNativeHandle(shaderAT);

    sfShader_setIntUniform(shaderAT, "nbLigth", 1);

    sfShader_setFloatUniform(shaderAT, "ligths[0].power", 5000000.);
    sfShader_setVec4Uniform(shaderAT, "ligths[0].color", (sfGlslVec4) { 1., 1., 1., 1. });
    sfShader_setVec3Uniform(shaderAT, "ligths[0].pos", (sfVector3f) { 0., 10000000., 0. });

    sfTexture* tutoTexture = sfTexture_createFromFile("..\\Ressource\\Level1 - TUTO\\wallTexture.png", NULL);
    GLuint textureID = sfTexture_getNativeHandle(tutoTexture);

    const unsigned int nbCarer = 10;

    Object3D* LevelFloor = creatNewObject((Color) {1., 1., 1., 1.}, 0, 0, 0, nbCarer * nbCarer * 2, GL_TRIANGLES, textureID, shaderIDAT, 0);

    unsigned int i = 0;
    for (int x = 0; x < nbCarer; x++) {
        for (int z = 0; z < nbCarer; z++) {
            LevelFloor->mesh[i].pos = (sfVector3f){ x, max(x, z), z};
            LevelFloor->mesh[i + 1].pos = (sfVector3f){ x + 1., max(x, z) + 1., z + 1. };
            LevelFloor->mesh[i + 2].pos = (sfVector3f){ x + 1., max(x + 1., z), z };
            LevelFloor->mesh[i + 3].pos = (sfVector3f){ x, max(x, z), z };
            LevelFloor->mesh[i + 4].pos = (sfVector3f){ x, max(x, z + 1.), z + 1. };
            LevelFloor->mesh[i + 5].pos = (sfVector3f){ x + 1., max(x, z) + 1., z + 1. };

            LevelFloor->mesh[i].texPos = (sfVector2f){ 0., 0. };
            LevelFloor->mesh[i + 1].texPos = (sfVector2f){ 1., 1. };
            LevelFloor->mesh[i + 2].texPos = (sfVector2f){ 1., 0. };
            LevelFloor->mesh[i + 3].texPos = (sfVector2f){ 0., 0. };
            LevelFloor->mesh[i + 4].texPos = (sfVector2f){ 0., 1. };
            LevelFloor->mesh[i + 5].texPos = (sfVector2f){ 1., 1. };

            LevelFloor->mesh[i].norm = (sfVector3f){ 0., 1., 0. };
            LevelFloor->mesh[i + 1].norm = (sfVector3f){ 0., 1., 0. };
            LevelFloor->mesh[i + 2].norm = (sfVector3f){ 0., 1., 0. };
            LevelFloor->mesh[i + 3].norm = (sfVector3f){ 0., 1., 0. };
            LevelFloor->mesh[i + 4].norm = (sfVector3f){ 0., 1., 0. };
            LevelFloor->mesh[i + 5].norm = (sfVector3f){ 0., 1., 0. };

            i += 6;
        }
    }

    for (i = 0; i < LevelFloor->nbVertex; i++) {
        LevelFloor->mesh[i].pos.y /= 4.5;
        LevelFloor->mesh[i].pos.y *= LevelFloor->mesh[i].pos.y;
    }

    creatMesh(LevelFloor, 1);

    Entity* player = createEntity((sfVector3f) { 1., 1., 1. });
    player->action = playerAction;
    
    float timer = 5.;
    
    while (sfRenderWindow_isOpen(window))
    {
        dt = sfTime_asSeconds(sfClock_restart(clock));
        timer += dt;

        Camera* cam = getCam();
        cam->x = player->pos.x;
        cam->y = player->pos.y;
        cam->z = player->pos.z;
        player->dir.x = cam->pitch;
        player->dir.y = cam->yaw;

        while (sfRenderWindow_pollEvent(window, &event))
        {
            switch (event.type) {
            case sfEvtClosed:
                sfRenderWindow_close(window);
                break;
            case sfEvtKeyPressed:
                switch (event.key.scancode) {
                case sfScanEscape:
                    sfRenderWindow_close(window);
                    break;
                case sfScanR:
                    timer = 0.;
                    break;
                }
                break;
            default:
                eventCamera(&event, window);
                break;
            }
        }

        updateAllEntity(dt);

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        updateLigth();
        object3D_render();

        sfRenderWindow_display(window);
    }

    sfClock_destroy(clock);
    sfRenderWindow_destroy(window);

    return 0;
}

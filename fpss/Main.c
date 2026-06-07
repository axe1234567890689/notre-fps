#include "Object3D.h"
#include "Entity.h"
#include "Player.h"
#include "LevelManager.h"

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

    initScene();

    loadScene(TUTO);

    sfClock* clock = sfClock_create();

    float cubeAngle = 0.f;
    float dt;
    sfEvent event;

    Entity* player = createEntity((sfVector3f) { 1., 10., 1. });
    player->action = playerAction;
    
    float timer = 5.;
    
    while (sfRenderWindow_isOpen(window))
    {
        dt = sfTime_asSeconds(sfClock_restart(clock));
        timer += dt;

        Camera* cam = getCam();
        cam->x = player->pos.x;
        cam->y = player->pos.y + 1.90;
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
        updateScenes(dt);

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        updateLigth();
        object3D_render();

        sfRenderWindow_display(window);
    }

    sfClock_destroy(clock);
    sfRenderWindow_destroy(window);

    return 0;
}

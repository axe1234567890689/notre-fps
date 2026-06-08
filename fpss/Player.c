#include "Object3D.h"
#include "Player.h"

void playerAction(Entity* _entity, float _dt)
{
    float speed = 500.f * _dt;

    sfVector3f speedVec = { 0., -9.31, 0. };

    if (sfKeyboard_isScancodePressed(sfScanW)) {
        speedVec.z -= speed * cosf(_entity->dir.y * DEG_TO_RAD);
        speedVec.x -= speed * sinf(_entity->dir.y * DEG_TO_RAD);
    }
    if (sfKeyboard_isScancodePressed(sfScanS)) {
        speedVec.z += speed * cosf(_entity->dir.y * DEG_TO_RAD);
        speedVec.x += speed * sinf(_entity->dir.y * DEG_TO_RAD);
    }
    if (sfKeyboard_isScancodePressed(sfScanA)) {
        speedVec.z += speed * sinf(_entity->dir.y * DEG_TO_RAD);
        speedVec.x -= speed * cosf(_entity->dir.y * DEG_TO_RAD);
    }
    if (sfKeyboard_isScancodePressed(sfScanD)) {
        speedVec.z -= speed * sinf(_entity->dir.y * DEG_TO_RAD);
        speedVec.x += speed * cosf(_entity->dir.y * DEG_TO_RAD);
    }
    
    float damping = 1.f / (1.f + 0.95 * _dt);

    speedVec = mulV3f(speedVec, _dt);

    sfVector3f exPos = _entity->pos;
    _entity->pos = getMouveVecCollid(_entity->pos, speedVec, 0);
}

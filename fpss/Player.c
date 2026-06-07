#include "Object3D.h"
#include "Player.h"

void playerAction(Entity* _entity, float _dt)
{
    float speed = 5.f * _dt;

    sfVector3f speedVec = { 0., 0., 0. };

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
    if (sfKeyboard_isScancodePressed(sfScanSpace)) {
        speedVec.y += speed;
    }
    if (sfKeyboard_isScancodePressed(sfScanLControl)) {
        speedVec.y -= speed;
    }

    sfVector3f exPos = _entity->pos;
    _entity->pos = getMouveVecCollid(_entity->pos, speedVec, 0);
    if (_entity->pos.y < -1.) _entity->pos.y = -1.;
}

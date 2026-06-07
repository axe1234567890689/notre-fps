#include "Object3D.h"
#include "Player.h"

void playerAction(Entity* _entity, float _dt)
{
    float speed = 5.f * _dt;

    _entity->speed.x += _entity->force.x * _dt;
    _entity->speed.y += (_entity->force.y - 9.81f) * _dt;
    _entity->speed.z += _entity->force.z * _dt;
    _entity->force.x -= _entity->force.x * _dt;
    _entity->force.y -= _entity->force.y * _dt;
    _entity->force.z -= _entity->force.z * _dt;
    sfVector3f speedVec = mulV3f(_entity->speed, _dt);

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

    sfVector3f exPos = _entity->pos;
    _entity->pos = getMouveVecCollid(_entity->pos, speedVec, 0);
    _entity->speed.x = (_entity->pos.x - exPos.x) / _dt;
    _entity->speed.y = (_entity->pos.y - exPos.y) / _dt;
    _entity->speed.z = (_entity->pos.z - exPos.z) / _dt;

    float dist = LENGTH(_entity->speed);

    if (dist > 1.) {
        _entity->speed.x *= (1. / dist);
        _entity->speed.z *= (1. / dist);
    }
}

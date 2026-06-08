#pragma once
#include "Object3D.h"

typedef struct Entity Entity;

struct Entity {
	Entity* next;
	sfVector3f pos;
	sfVector2f dir;
	void (*action) (Entity* _entity, float _dt);
	void* data;
};

Entity* createEntity(sfVector3f _pos);

void updateAllEntity(float _dt);

void moveEntity(Entity* _entity, sfVector3f _mouve);

#include "Entity.h"

Entity* firstEntity = NULL;

Entity* createEntity(sfVector3f _pos)
{
	Entity* newEntity = calloc(1, sizeof(Entity));
	newEntity->pos = _pos;
	newEntity->data = NULL;
	newEntity->next = firstEntity;
	firstEntity = newEntity;
	return newEntity;
}

void updateAllEntity(float _dt)
{
	Entity* current = firstEntity;
	while (current != NULL) {
		current->action(current, _dt);

		current = current->next;
	}
}

void moveEntity(Entity* _entity, sfVector3f _mouve)
{
	getMouveVecCollid(_entity->pos, addVec3(_entity->pos, _mouve), 0);
}

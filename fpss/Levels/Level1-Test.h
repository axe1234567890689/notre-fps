#pragma once
#include "../LevelManager.h"

typedef struct Level1_Data {
	sfShader* basicLigth;
	Object3D* testModel;

	float timer;
} Level1_Data;

void Level1_init(Scene* _scene);

void Level1_update(Scene* _scene, float _dt);

void Level1_destroy(Scene* _scene);

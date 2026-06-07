#pragma once
#include "../LevelManager.h"

typedef struct Tuto_Data {
	sfTexture* tutoTexture;
	sfShader* shaderAT;
	Object3D* LevelFloor;
	Object3D* LevelDoor;

	char transi;

	float timer;
} Tuto_Data;

void Tuto_init(Scene* _scene);

void Tuto_update(Scene* _scene, float _dt);

void Tuto_destroy(Scene* _scene);

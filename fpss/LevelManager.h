#pragma once
#include "Object3D.h"

#define NB_SCENE 1

typedef char SceneType;
enum SceneType {
	TUTO,
};

typedef struct Scene Scene;
struct Scene {
	Scene* next;
	SceneType type;
	void (*update)(Scene* _scene, float _dt);
	void* data;
};

void initScene();

void addSceneType(SceneType _scene, void(*_init[NB_SCENE])(Scene* _scene), void(*_update)(Scene* _scene, float _dt), void(*_destroy[NB_SCENE])(Scene* _scene));

void loadScene(SceneType _scene);

void updateScenes(float _dt);

void destroyOneScene(SceneType _scene);

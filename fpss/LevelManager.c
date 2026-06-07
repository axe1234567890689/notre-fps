#include "LevelManager.h"
#include "Levels/Level0-Tuto.h"
#include "Levels/Level1-Test.h"

Scene* firstScene = NULL;
void (*init[NB_SCENE])(Scene* _scene);
void (*updates[NB_SCENE])(Scene* _scene, float _dt);
void (*destroy[NB_SCENE])(Scene* _scene);

void initScene() {
	addSceneType(TUTO, Tuto_init, Tuto_update, Tuto_destroy);
	addSceneType(Level1, Level1_init, Level1_update, Level1_destroy);
}

void addSceneType(SceneType _type, void(*_init[NB_SCENE])(Scene* _scene), void(*_update)(Scene* _scene, float _dt), void(*_destroy[NB_SCENE])(Scene* _scene))
{
	init[_type] = _init;
	updates[_type] = _update;
	destroy[_type] = _destroy;
}

void loadScene(SceneType _scene)
{
	Scene* newScene = calloc(1, sizeof(Scene));

	newScene->type = _scene;
	newScene->update = updates[_scene];

	init[_scene](newScene);

	newScene->next = firstScene;
	firstScene = newScene;
}

void updateScenes(float _dt)
{
	Scene* current = firstScene;
	while (current != NULL) {
		current->update(current, _dt);

		current = current->next;
	}
}

void destroyOneScene(SceneType _scene)
{
	Scene* last = NULL;
	Scene* current = firstScene;
	while (current != NULL) {
		if (current->type == _scene) {
			destroy[_scene](current);
			if (last == NULL) {
				firstScene = current->next;
				free(current);
				return;
			}
			last->next = current->next;
			free(current);
			return;
		}

		last = current;
		current = current->next;
	}
}

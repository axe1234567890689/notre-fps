#include "Level1-Test.h"

void Level1_init(Scene* _scene)
{
    _scene->data = calloc(1, sizeof(Level1_Data));

    Level1_Data* data = _scene->data;
    data->timer = -1.f;

    data->basicLigth = sfShader_createFromFile("..\\Ressource\\Basic\\ambient.vert", NULL, "..\\Ressource\\Basic\\ambient.frag");
    GLuint shaderIDAT = sfShader_getNativeHandle(data->basicLigth);

    sfShader_setIntUniform(data->basicLigth, "nbLigth", 1);

    sfShader_setFloatUniform(data->basicLigth, "ligths[0].power", 5000000.);
    sfShader_setVec4Uniform(data->basicLigth, "ligths[0].color", (sfGlslVec4) { 1., 1., 1., 1. });
    sfShader_setVec3Uniform(data->basicLigth, "ligths[0].pos", (sfVector3f) { 0., 100000., 0. });

    const unsigned int nbCarer = 10;

    data->testModel = loadModel("..\\Ressource\\Level1 - tktJe c pas\\models\\suzanne.data", GL_TRIANGLES, 20., shaderIDAT, 0, 2);
}

void Level1_update(Scene* _scene, float _dt)
{
    Level1_Data* data = _scene->data;

    data->timer += _dt * 2.;

    //data->testModel->roll = data->timer;
    //data->testModel->pitch = data->timer;
    updateCollider(data->testModel);
}

void Level1_destroy(Scene* _scene)
{
    Level1_Data* data = _scene->data;

    sfShader_destroy(data->basicLigth);

    object3D_destroy(data->testModel);
}

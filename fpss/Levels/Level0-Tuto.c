#include "Level0-Tuto.h"

void Tuto_init(Scene* _scene)
{
    _scene->data = calloc(1, sizeof(Tuto_Data));

    Tuto_Data* data = _scene->data;
    data->timer = -1.f;

    data->tutoTexture = sfTexture_createFromFile("..\\Ressource\\Level0 - TUTO\\wallTexture.png", NULL);
    sfTexture_setRepeated(data->tutoTexture, sfTrue);
    GLuint textureID = sfTexture_getNativeHandle(data->tutoTexture);

    sfTexture* texturertg = sfTexture_createFromFile("..\\Ressource\\Level1 - tktJe c pas\\floor.png", NULL);
    sfTexture_setRepeated(texturertg, sfTrue);
    GLuint texture2 = sfTexture_getNativeHandle(texturertg);

    data->shaderAT = sfShader_createFromFile("..\\Ressource\\Level0 - TUTO\\ambient.vert", NULL, "..\\Ressource\\Level0 - TUTO\\WallTransition.frag");
    GLuint shaderIDAT = sfShader_getNativeHandle(data->shaderAT); 
    
    glUseProgram(shaderIDAT);

    glUniform1i(glGetUniformLocation(shaderIDAT, "texTuto"), 0);
    glUniform1i(glGetUniformLocation(shaderIDAT, "texLevel2"), 1);

    glUniform2f(glGetUniformLocation(shaderIDAT, "transiPos"), 5., 10.);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    sfShader_setFloatUniform(data->shaderAT, "transiDist", data->timer);

    sfShader_setIntUniform(data->shaderAT, "nbLigth", 1);

    sfShader_setFloatUniform(data->shaderAT, "ligths[0].power", 5000000.);
    sfShader_setVec4Uniform(data->shaderAT, "ligths[0].color", (sfGlslVec4) { 1., 1., 1., 1. });
    sfShader_setVec3Uniform(data->shaderAT, "ligths[0].pos", (sfVector3f) { 0., 100000., 0. });

    const unsigned int nbCarer = 10;

    Object3D* LevelFloor = creatNewObject(0, 0, 0, nbCarer * nbCarer * 2, GL_TRIANGLES, textureID, shaderIDAT, 0);

    unsigned int i = 0;
    for (int x = 0; x < nbCarer; x++) {
        for (int z = 0; z < nbCarer; z++) {
            
            LevelFloor->mesh[i].pos = (sfVector3f){ x, max(x, z), z };
            LevelFloor->mesh[i + 1].pos = (sfVector3f){ x + 1., max(x, z) + 1., z + 1. };
            LevelFloor->mesh[i + 2].pos = (sfVector3f){ x + 1., max(x + 1., z), z };
            LevelFloor->mesh[i + 3].pos = (sfVector3f){ x, max(x, z), z };
            LevelFloor->mesh[i + 4].pos = (sfVector3f){ x, max(x, z + 1.), z + 1. };
            LevelFloor->mesh[i + 5].pos = (sfVector3f){ x + 1., max(x, z) + 1., z + 1. };

            LevelFloor->mesh[i].texPos = (sfVector2f){ 0., 0. };
            LevelFloor->mesh[i + 1].texPos = (sfVector2f){ 1., 1. };
            LevelFloor->mesh[i + 2].texPos = (sfVector2f){ 1., 0. };
            LevelFloor->mesh[i + 3].texPos = (sfVector2f){ 0., 0. };
            LevelFloor->mesh[i + 4].texPos = (sfVector2f){ 0., 1. };
            LevelFloor->mesh[i + 5].texPos = (sfVector2f){ 1., 1. };

            i += 6;
        }
    }

    for (i = 0; i < LevelFloor->nbVertex; i++) {
        LevelFloor->mesh[i].pos.y /= 4.;
        LevelFloor->mesh[i].pos.y *= LevelFloor->mesh[i].pos.y;
    }

    for (i = 0; i < LevelFloor->nbVertex; i += 3) {
        LevelFloor->mesh[i].norm = computeNormal(LevelFloor->mesh[i].pos, LevelFloor->mesh[i + 1].pos, LevelFloor->mesh[i + 2].pos);
        LevelFloor->mesh[i + 1].norm = LevelFloor->mesh[i].norm;
        LevelFloor->mesh[i + 2].norm = LevelFloor->mesh[i].norm;
    }

    creatMesh(LevelFloor, 1);

    data->LevelWall = creatNewObject(0, 0, 0, 8, GL_TRIANGLES, textureID, shaderIDAT, 0);

    data->LevelWall->mesh[0].pos = (sfVector3f){ 0., 0., 0. };
    data->LevelWall->mesh[0].texPos = (sfVector2f){ 0., 0. };

    data->LevelWall->mesh[1].pos = (sfVector3f){ 0., 2., nbCarer };
    data->LevelWall->mesh[1].texPos = (sfVector2f){ 2., nbCarer };

    data->LevelWall->mesh[2].pos = (sfVector3f){ 0., 0., nbCarer };
    data->LevelWall->mesh[2].texPos = (sfVector2f){ 0., nbCarer };

    data->LevelWall->mesh[3].pos = (sfVector3f){ 0., 0., 0. };
    data->LevelWall->mesh[3].texPos = (sfVector2f){ 0., 0. };

    data->LevelWall->mesh[4].pos = (sfVector3f){ 0., 2., 0. };
    data->LevelWall->mesh[4].texPos = (sfVector2f){ 2., 0. };

    data->LevelWall->mesh[5].pos = (sfVector3f){ 0., 2., nbCarer };
    data->LevelWall->mesh[5].texPos = (sfVector2f){ 2., nbCarer };

    data->LevelWall->mesh[6].pos = (sfVector3f){ 0., 0., 0. };
    data->LevelWall->mesh[6].texPos = (sfVector2f){ 0., nbCarer };

    data->LevelWall->mesh[7].pos = (sfVector3f){ nbCarer, 0., 0. };
    data->LevelWall->mesh[7].texPos = (sfVector2f){ 0., 0. };

    data->LevelWall->mesh[8].pos = (sfVector3f){ nbCarer, 2., 0. };
    data->LevelWall->mesh[8].texPos = (sfVector2f){ 2., 0. };

    data->LevelWall->mesh[9].pos = (sfVector3f){ 0., 0., 0. };
    data->LevelWall->mesh[9].texPos = (sfVector2f){ 0., nbCarer };

    data->LevelWall->mesh[10].pos = (sfVector3f){ nbCarer, 2., 0. };
    data->LevelWall->mesh[10].texPos = (sfVector2f){ 2., 0. };

    data->LevelWall->mesh[11].pos = (sfVector3f){ 0., 2., 0. };
    data->LevelWall->mesh[11].texPos = (sfVector2f){ 2., nbCarer };

    data->LevelWall->mesh[12].pos = (sfVector3f){ nbCarer, 0., nbCarer };
    data->LevelWall->mesh[12].texPos = (sfVector2f){ 0., 0. };

    data->LevelWall->mesh[13].pos = (sfVector3f){ 0., 0., nbCarer };
    data->LevelWall->mesh[13].texPos = (sfVector2f){ 0., nbCarer };

    data->LevelWall->mesh[14].pos = (sfVector3f){ 0., 2., nbCarer };
    data->LevelWall->mesh[14].texPos = (sfVector2f){ 2., nbCarer };

    data->LevelWall->mesh[15].pos = (sfVector3f){ nbCarer, 0., nbCarer };
    data->LevelWall->mesh[15].texPos = (sfVector2f){ 0., 0. };

    data->LevelWall->mesh[16].pos = (sfVector3f){ 0., 2., nbCarer };
    data->LevelWall->mesh[16].texPos = (sfVector2f){ 2., nbCarer };

    data->LevelWall->mesh[17].pos = (sfVector3f){ nbCarer, 2., nbCarer };
    data->LevelWall->mesh[17].texPos = (sfVector2f){ 2., 0. };

    data->LevelWall->mesh[18].pos = (sfVector3f){ nbCarer, 0., nbCarer };
    data->LevelWall->mesh[18].texPos = (sfVector2f){ 0., nbCarer };

    data->LevelWall->mesh[19].pos = (sfVector3f){ nbCarer, 2., 0. };
    data->LevelWall->mesh[19].texPos = (sfVector2f){ 2., 0. };

    data->LevelWall->mesh[20].pos = (sfVector3f){ nbCarer, 0., 0. };
    data->LevelWall->mesh[20].texPos = (sfVector2f){ 0., 0. };

    data->LevelWall->mesh[21].pos = (sfVector3f){ nbCarer, 0., nbCarer };
    data->LevelWall->mesh[21].texPos = (sfVector2f){ 0., nbCarer };

    data->LevelWall->mesh[22].pos = (sfVector3f){ nbCarer, 2., nbCarer };
    data->LevelWall->mesh[22].texPos = (sfVector2f){ 2., nbCarer };

    data->LevelWall->mesh[23].pos = (sfVector3f){ nbCarer, 2., 0. };
    data->LevelWall->mesh[23].texPos = (sfVector2f){ 2., 0. };

    for (i = 0; i < data->LevelWall->nbVertex; i += 3) {
        data->LevelWall->mesh[i].norm = computeNormal(data->LevelWall->mesh[i].pos, data->LevelWall->mesh[i + 1].pos, data->LevelWall->mesh[i + 2].pos);
        data->LevelWall->mesh[i + 1].norm = data->LevelWall->mesh[i].norm;
        data->LevelWall->mesh[i + 2].norm = data->LevelWall->mesh[i].norm;
    }

    creatMesh(data->LevelWall, 1);

    data->LevelDoor = creatNewObject(0, 0, 0, 6, GL_TRIANGLES, textureID, shaderIDAT, 0);

    data->LevelDoor->mesh[0].pos = (sfVector3f){ -0.5, 0., -0.5 };
    data->LevelDoor->mesh[0].norm = (sfVector3f){ -1., 0., 0. };
    data->LevelDoor->mesh[0].texPos = (sfVector2f){ 0., 0. };

    data->LevelDoor->mesh[1].pos = (sfVector3f){ 0.5, 2., -0.5 };
    data->LevelDoor->mesh[1].norm = (sfVector3f){ -1., 0., 0. };
    data->LevelDoor->mesh[1].texPos = (sfVector2f){ 1., 2. };

    data->LevelDoor->mesh[2].pos = (sfVector3f){ 0.5, 0., -0.5 };
    data->LevelDoor->mesh[2].norm = (sfVector3f){ -1., 0., 0. };
    data->LevelDoor->mesh[2].texPos = (sfVector2f){ 1., 0. };

    data->LevelDoor->mesh[3].pos = (sfVector3f){ -0.5, 0., -0.5 };
    data->LevelDoor->mesh[3].norm = (sfVector3f){ -1., 0., 0. };
    data->LevelDoor->mesh[3].texPos = (sfVector2f){ 0., 0. };

    data->LevelDoor->mesh[4].pos = (sfVector3f){ -0.5, 2., -0.5 };
    data->LevelDoor->mesh[4].norm = (sfVector3f){ -1., 0., 0. };
    data->LevelDoor->mesh[4].texPos = (sfVector2f){ 0., 2. };

    data->LevelDoor->mesh[5].pos = (sfVector3f){ 0.5, 2., -0.5 };
    data->LevelDoor->mesh[5].norm = (sfVector3f){ -1., 0., 0. };
    data->LevelDoor->mesh[5].texPos = (sfVector2f){ 1., 2. };

    data->LevelDoor->mesh[6].pos = (sfVector3f){ -0.5, 0., 0. };
    data->LevelDoor->mesh[6].norm = (sfVector3f){ 1., 0., 0. };
    data->LevelDoor->mesh[6].texPos = (sfVector2f){ 0., 0. };

    data->LevelDoor->mesh[7].pos = (sfVector3f){ 0.5, 0., 0. };
    data->LevelDoor->mesh[7].norm = (sfVector3f){ 1., 0., 0. };
    data->LevelDoor->mesh[7].texPos = (sfVector2f){ 1., 0. };

    data->LevelDoor->mesh[8].pos = (sfVector3f){ 0.5, 2., 0. };
    data->LevelDoor->mesh[8].norm = (sfVector3f){ 1., 0., 0. };
    data->LevelDoor->mesh[8].texPos = (sfVector2f){ 1., 2. };

    data->LevelDoor->mesh[9].pos = (sfVector3f){ -0.5, 0., 0. };
    data->LevelDoor->mesh[9].norm = (sfVector3f){ 1., 0., 0. };
    data->LevelDoor->mesh[9].texPos = (sfVector2f){ 0., 0. };

    data->LevelDoor->mesh[10].pos = (sfVector3f){ 0.5, 2., 0. };
    data->LevelDoor->mesh[10].norm = (sfVector3f){ 1., 0., 0. };
    data->LevelDoor->mesh[10].texPos = (sfVector2f){ 1., 2. };

    data->LevelDoor->mesh[11].pos = (sfVector3f){ -0.5, 2., 0. };
    data->LevelDoor->mesh[11].norm = (sfVector3f){ 1., 0., 0. };
    data->LevelDoor->mesh[11].texPos = (sfVector2f){ 0., 2. };

    data->LevelDoor->pos = (sfVector3f) { 5., 4.94, 10.5 };

    for (i = 0; i < data->LevelDoor->nbVertex; i += 3) {
        data->LevelDoor->mesh[i].norm = computeNormal(data->LevelDoor->mesh[i].pos, data->LevelDoor->mesh[i + 1].pos, data->LevelDoor->mesh[i + 2].pos);
        data->LevelDoor->mesh[i + 1].norm = data->LevelDoor->mesh[i].norm;
        data->LevelDoor->mesh[i + 2].norm = data->LevelDoor->mesh[i].norm;
    }

    creatMesh(data->LevelDoor, 1);

    data->LevelFloor = LevelFloor;

    data->transi = 0;
}

void Tuto_update(Scene* _scene, float _dt)
{
    Tuto_Data* data = _scene->data;

    if (data->transi) {
        data->timer += _dt * 3.;

        sfShader_setFloatUniform(data->shaderAT, "transiDist", data->timer);
        if (data->timer < 1.5) {
            data->LevelDoor->pos = (sfVector3f){ 5. + data->timer / 1.5 + 1., 4.94, 10.5 };
            updateCollider(data->LevelDoor);
        }

        //data->LevelDoor->yaw = data->timer;
        //data->LevelDoor->roll = data->timer;
        //data->LevelFloor->pitch = data->timer;
        //updateCollider(data->LevelFloor);
    }
    else {
        if (sfKeyboard_isScancodePressed(sfScanF)) {
            data->transi = 1;
        }
    }
}

void Tuto_destroy(Scene* _scene)
{
    Tuto_Data* data = _scene->data;

    sfRenderTexture_destroy(data->tutoTexture);
    sfShader_destroy(data->shaderAT);

    object3D_destroy(data->LevelFloor);
}

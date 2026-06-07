#include "Object3D.h"
#include <stdlib.h>

static Camera camera =
{
    0.f,
    0.f,
    5.f,

    0.f,
    0.f
};

char keyPressed;
#ifdef DEBUG
Object3D* viewCollisions;
#endif // DEBUG

Object3D* firstObject3D = NULL;
CollideMesh* firstCollider = NULL;
Ligth ligths[10];
int nbLigths = 0;
Mat4 proj;
sfShader* shaderBasic;
GLuint shaderIDBasic;

void readText(FILE* _file, char _link[256]) {
    char c = fgetc(_file);
    while (c != '\"') {
        c = fgetc(_file);
    }
    unsigned int i = 0;
    c = fgetc(_file);
    while (c != '\"') {
        _link[i] = c;
        c = fgetc(_file);
        i++;
    }
    _link[i] = '\0';
}

Ligth* createLigth(Color _color, sfVector3f _pos, float _power)
{
    ligths[nbLigths].color = _color;
    ligths[nbLigths].pos = _pos;
    ligths[nbLigths].power = _power;
    nbLigths++;
    return &ligths[nbLigths - 1];
}

CollideMesh* creatCollider() {
    CollideMesh* newCollider = calloc(1, sizeof(CollideMesh));
    newCollider->next = firstCollider;
    firstCollider = newCollider;
    return newCollider;
}

Object3D* loadModel(const char* _link, GLuint _renderType, float _scale, GLuint _shader, ShaderParam _shaderParam, char _hasTrueCollider)
{
    unsigned int nbPos = 0, nbTexts = 0, nbNormals = 0, nbFaces = 0;
    GLuint Texture = 0;
    FILE* dataFile = fopen(_link, "r");
    fscanf_s(dataFile, "Poss : %d\n", &nbPos);
    fscanf_s(dataFile, "Texts : %d\n", &nbTexts);
    fscanf_s(dataFile, "Normals : %d\n", &nbNormals);
    fscanf_s(dataFile, "Faces : %d\n", &nbFaces);
    char textureLink[256];
    readText(dataFile, textureLink);
    char modelLink[256];
    readText(dataFile, modelLink);
    if (textureLink[0] != '\0') {
        Texture = sfTexture_getNativeHandle(sfTexture_createFromFile(textureLink, NULL));
    }
    fclose(dataFile);

    Object3D* model = creatNewObject(nbPos, nbTexts, nbNormals, nbFaces, _renderType, Texture, _shader, _shaderParam);

    FILE* modelFile = fopen(modelLink, "r");

    unsigned int index[12];
    unsigned int currentPos = 0, currentTex = 0, currentNormal = 0, currentFace = 0;

    char c = fgetc(modelFile);
    while (c != EOF) {
        switch (c) {
        case ' ':
        case '#':
            while (c != '\n' && c != EOF)
                c = fgetc(modelFile);
            break;
        case 'v':
            c = fgetc(modelFile);
            switch (c) {
            case ' ':
                fscanf_s(modelFile, "%f %f %f", &model->poss[currentPos].x, &model->poss[currentPos].y, &model->poss[currentPos].z);
                model->poss[currentPos].x *= _scale;
                model->poss[currentPos].y *= _scale;
                model->poss[currentPos].z *= _scale;
                currentPos++;
                break;
            case 'n':
                fscanf_s(modelFile, " %f %f %f", &model->normals[currentNormal].x, &model->normals[currentNormal].y, &model->normals[currentNormal].z);
                currentNormal++;
                break;
            case 't':
                fscanf_s(modelFile, " %f %f", &model->texs[currentTex].x, &model->texs[currentTex].y);
                currentTex++;
                break;
            }
            break;
        case 'f':
            for (int i = 0; i < 12 && c != '\n'; i++) {
                if (fscanf_s(modelFile, "%d", &index[i]) == 0) {
                    index[i] = 0;
                } else {
                    index[i]--;
                }
                c = fgetc(modelFile);
                if (c == ' ') {
                    while ((i + 1) % 3 != 0) {
                        i++;
                        index[i] = 0;
                    }
                }
                if (i == 11) {
                    model->faces[currentFace].corners[0].posIndex = index[0];
                    model->faces[currentFace].corners[0].textIndex = index[1];
                    model->faces[currentFace].corners[0].normalIndex = index[2];
                    model->faces[currentFace].corners[2].posIndex = index[9];
                    model->faces[currentFace].corners[2].textIndex = index[10];
                    model->faces[currentFace].corners[2].normalIndex = index[11];
                    model->faces[currentFace].corners[1].posIndex = index[6];
                    model->faces[currentFace].corners[1].textIndex = index[7];
                    model->faces[currentFace].corners[1].normalIndex = index[8];
                    currentFace++;
                }
                if (c == '\n') {
                    while (i < 8) {
                        i++;
                        index[i] = 0;
                    }
                }
            }
            model->faces[currentFace].corners[0].posIndex = index[0];
            model->faces[currentFace].corners[0].textIndex = index[1];
            model->faces[currentFace].corners[0].normalIndex = index[2];
            model->faces[currentFace].corners[1].posIndex = index[3];
            model->faces[currentFace].corners[1].textIndex = index[4];
            model->faces[currentFace].corners[1].normalIndex = index[5];
            model->faces[currentFace].corners[2].posIndex = index[6];
            model->faces[currentFace].corners[2].textIndex = index[7];
            model->faces[currentFace].corners[2].normalIndex = index[8];
            currentFace++;
            break;
        }

        c = fgetc(modelFile);
    }

    for (int i = 0; i < nbFaces; i++) {
        for (int j = 0; j < 3; j++) {
            model->mesh[i * 3 + j].pos = model->poss[model->faces[i].corners[j].posIndex];
            model->mesh[i * 3 + j].norm = model->normals[model->faces[i].corners[j].normalIndex];
            model->mesh[i * 3 + j].texPos = model->texs[model->faces[i].corners[j].textIndex];
        }
    }

    model->nbVertex = nbFaces * 3; 
    
    creatMesh(model, _hasTrueCollider);

    return model;
}

Object3D* creatNewObject(unsigned int _nbPos, unsigned int _nbText, unsigned int _nbNormals, unsigned int _nbFaces, GLuint _renderType, GLuint _texture, GLuint _shaderID, ShaderParam _shaderParam) {
    Object3D* newObject = calloc(1, sizeof(Object3D));
    newObject->pos = (sfVector3f){ 0., 0., 0. };
    newObject->nbFaces = _nbFaces;
    newObject->texture = _texture;
    newObject->poss = calloc(_nbPos, sizeof(sfVector3f));
    newObject->texs = calloc(_nbText, sizeof(sfVector2f));
    newObject->normals = calloc(_nbNormals, sizeof(sfVector3f));
    newObject->faces = calloc(_nbFaces, sizeof(Face));
    newObject->nbVertex = _nbFaces * 3;
    newObject->renderType = _renderType;
    newObject->mesh = calloc(newObject->nbVertex, sizeof(Vertex));
    setShader(newObject, _shaderID, _shaderParam);
    newObject->next = firstObject3D;
    firstObject3D = newObject;
    return newObject;
}

void eventCamera(sfEvent* event, sfRenderWindow* window)
{
    switch (event->type) {
    case sfEvtMouseMoved:
        camera.pitch -= ((event->mouseMove.y - 540.) / 540.) * 100.f;
        camera.yaw -= ((event->mouseMove.x - 960.) / 960.) * 100.f;
        sfMouse_setPosition((sfVector2i) { 960, 540 }, window);
        break;
    }
}

Mat4 perspective(float _fov, float _aspect, float _near, float _far)
{
    Mat4 out = { 0 };

    float f = 1.0f / tanf(_fov * 0.5f * 3.141592f / 180.f);

    out.m[0] = f / _aspect;
    out.m[5] = f;
    out.m[10] = (_far + _near) / (_near - _far);
    out.m[11] = -1.f;
    out.m[14] = (2.f * _far * _near) / (_near - _far);

    return out;
}

Mat4 viewMatrix(Camera* _camera)
{
    Mat4 v = { 0 };

    float cosP = cosf(-_camera->pitch * DEG_TO_RAD);
    float sinP = sinf(-_camera->pitch * DEG_TO_RAD);
    float cosY = cosf(_camera->yaw * DEG_TO_RAD);
    float sinY = sinf(_camera->yaw * DEG_TO_RAD);

    // forward
    float fx = cosP * sinY;
    float fy = sinP;
    float fz = cosP * cosY;

    // right
    float rx = cosY;
    float rz = -sinY;

    // up (simplifié)
    float ux = -sinP * sinY;
    float uy = cosP;
    float uz = -sinP * cosY;

    v.m[0] = rx;
    v.m[1] = ux;
    v.m[2] = fx;

    v.m[4] = 0;
    v.m[5] = uy;
    v.m[6] = fy;

    v.m[8] = rz;
    v.m[9] = uz;
    v.m[10] = fz;

    v.m[15] = 1;

    v.m[12] = -(_camera->x * rx + _camera->y * v.m[4] + _camera->z * rz);
    v.m[13] = -(_camera->x * ux + _camera->y * uy + _camera->z * uz);
    v.m[14] = -(_camera->x * fx + _camera->y * fy + _camera->z * fz);

    return v;
}

Mat4 identity(Object3D* _model) {
    Mat4 out = { 0 };
    out.m[12] = _model->pos.x;
    out.m[13] = _model->pos.y;
    out.m[14] = _model->pos.z;
    
    out.m[0] = 1;
    out.m[5] = 1;
    out.m[10] = 1;
    out.m[15] = 1;

    float cx = cosf(_model->pitch);
    float sx = sinf(_model->pitch);

    float cy = cosf(_model->yaw);
    float sy = sinf(_model->yaw);

    float cz = cosf(_model->roll);
    float sz = sinf(_model->roll);

    out.m[5] = cx * cz;
    out.m[6] = sx;

    out.m[9] = -sx;
    out.m[10] = cx * cy;

    out.m[0] = cy * cz;
    out.m[2] = -sy;

    out.m[8] = sy;

    out.m[1] = sz;

    out.m[4] = -sz;

    return out;
}

sfVector3f addVec3(sfVector3f _a, sfVector3f _b) {
    return (sfVector3f) { _a.x + _b.x, _a.y + _b.y, _a.z + _b.z };
}

void object3D_destroy(Object3D* _object)
{
}

void updateCollider(Object3D* _object) {
    CollideMesh* collider = _object->collider;
    Mat4 model = identity(_object);

    for (int i = 0; i < collider->nbFace; i++) {
        unsigned int j = i * 3;
        collider->faces[i].points[0].x = _object->mesh[j].pos.x * model.m[0] + _object->mesh[j].pos.y * model.m[4] + _object->mesh[j].pos.z * model.m[8];
        collider->faces[i].points[0].y = _object->mesh[j].pos.x * model.m[1] + _object->mesh[j].pos.y * model.m[5] + _object->mesh[j].pos.z * model.m[9];
        collider->faces[i].points[0].z = _object->mesh[j].pos.x * model.m[2] + _object->mesh[j].pos.y * model.m[6] + _object->mesh[j].pos.z * model.m[10];
        collider->faces[i].points[0] = addVec3(collider->faces[i].points[0], _object->pos);
        j++;
        collider->faces[i].points[1].x = _object->mesh[j].pos.x * model.m[0] + _object->mesh[j].pos.y * model.m[4] + _object->mesh[j].pos.z * model.m[8];
        collider->faces[i].points[1].y = _object->mesh[j].pos.x * model.m[1] + _object->mesh[j].pos.y * model.m[5] + _object->mesh[j].pos.z * model.m[9];
        collider->faces[i].points[1].z = _object->mesh[j].pos.x * model.m[2] + _object->mesh[j].pos.y * model.m[6] + _object->mesh[j].pos.z * model.m[10];
        collider->faces[i].points[1] = addVec3(collider->faces[i].points[1], _object->pos);
        j++;
        collider->faces[i].points[2].x = _object->mesh[j].pos.x * model.m[0] + _object->mesh[j].pos.y * model.m[4] + _object->mesh[j].pos.z * model.m[8];
        collider->faces[i].points[2].y = _object->mesh[j].pos.x * model.m[1] + _object->mesh[j].pos.y * model.m[5] + _object->mesh[j].pos.z * model.m[9];
        collider->faces[i].points[2].z = _object->mesh[j].pos.x * model.m[2] + _object->mesh[j].pos.y * model.m[6] + _object->mesh[j].pos.z * model.m[10];
        collider->faces[i].points[2] = addVec3(collider->faces[i].points[2], _object->pos);

        collider->faces[i].normal = (sfVector3f){
            _object->mesh[i * 3].norm.x + _object->mesh[i * 3 + 1].norm.x + _object->mesh[i * 3 + 2].norm.x,
            _object->mesh[i * 3].norm.y + _object->mesh[i * 3 + 1].norm.y + _object->mesh[i * 3 + 2].norm.y,
            _object->mesh[i * 3].norm.z + _object->mesh[i * 3 + 1].norm.z + _object->mesh[i * 3 + 2].norm.z };

        float dist = sqrtf(collider->faces[i].normal.x * collider->faces[i].normal.x + collider->faces[i].normal.y * collider->faces[i].normal.y + collider->faces[i].normal.z * collider->faces[i].normal.z);

        collider->faces[i].normal.x /= dist;
        collider->faces[i].normal.y /= dist;
        collider->faces[i].normal.z /= dist;

        collider->faces[i].box.minX = min(min(collider->faces[i].points[0].x, collider->faces[i].points[1].x), collider->faces[i].points[2].x);
        collider->faces[i].box.minY = min(min(collider->faces[i].points[0].y, collider->faces[i].points[1].y), collider->faces[i].points[2].y);
        collider->faces[i].box.minZ = min(min(collider->faces[i].points[0].z, collider->faces[i].points[1].z), collider->faces[i].points[2].z);
        collider->faces[i].box.sizeX = max(max(collider->faces[i].points[0].x, collider->faces[i].points[1].x), collider->faces[i].points[2].x) - collider->faces[i].box.minX;
        collider->faces[i].box.sizeY = max(max(collider->faces[i].points[0].y, collider->faces[i].points[1].y), collider->faces[i].points[2].y) - collider->faces[i].box.minY;
        collider->faces[i].box.sizeZ = max(max(collider->faces[i].points[0].z, collider->faces[i].points[1].z), collider->faces[i].points[2].z) - collider->faces[i].box.minZ;
    }

    collider->box = collider->faces[0].box;
    for (int i = 1; i < collider->nbFace; i++) {
        if (collider->faces[i].box.minX < collider->box.minX) {
            collider->box.sizeX += collider->box.minX - collider->faces[i].box.minX;
            collider->box.minX = collider->faces[i].box.minX;
        }
        if (collider->faces[i].box.minY < collider->box.minY) {
            collider->box.sizeY += collider->box.minY - collider->faces[i].box.minY;
            collider->box.minY = collider->faces[i].box.minY;
        }
        if (collider->faces[i].box.minZ < collider->box.minZ) {
            collider->box.sizeZ += collider->box.minZ - collider->faces[i].box.minZ;
            collider->box.minZ = collider->faces[i].box.minZ;
        }

        if (collider->faces[i].box.minX + collider->faces[i].box.sizeX > collider->box.minX + collider->box.sizeX) {
            collider->box.sizeX = (collider->faces[i].box.minX + collider->faces[i].box.sizeX) - collider->box.minX;
        }
        if (collider->faces[i].box.minY + collider->faces[i].box.sizeY > collider->box.minY + collider->box.sizeY) {
            collider->box.sizeY = (collider->faces[i].box.minY + collider->faces[i].box.sizeY) - collider->box.minY;
        }
        if (collider->faces[i].box.minZ + collider->faces[i].box.sizeZ > collider->box.minZ + collider->box.sizeZ) {
            collider->box.sizeZ = (collider->faces[i].box.minZ + collider->faces[i].box.sizeZ) - collider->box.minZ;
        }
    }

    if (_object->colliderVisual != NULL) {
        updateCollideVisual(_object);

        updateMesh(_object->colliderVisual);
    }
}

void updateCollideVisual(Object3D* _object) {
    Object3D* collideObject = _object->colliderVisual;
    unsigned int i = 0;
    for (int j = 0; j < _object->collider->nbFace; j++) {
        collideObject->mesh[i].pos = _object->collider->faces[j].points[0];
        collideObject->mesh[i + 1].pos = _object->collider->faces[j].points[1];
        collideObject->mesh[i + 2].pos = _object->collider->faces[j].points[1];
        collideObject->mesh[i + 3].pos = _object->collider->faces[j].points[2];
        collideObject->mesh[i + 4].pos = _object->collider->faces[j].points[2];
        collideObject->mesh[i + 5].pos = _object->collider->faces[j].points[0];

        i += 6;
    }
    /*
    for (unsigned int j = 0; j < _object->collider->nbFace; j++) {
        sfVector3f startPos = (sfVector3f){ _object->collider->faces[j].box.minX, _object->collider->faces[j].box.minY, _object->collider->faces[j].box.minZ };
        sfVector3f endPos = (sfVector3f){ _object->collider->faces[j].box.minX + _object->collider->faces[j].box.sizeX, _object->collider->faces[j].box.minY + _object->collider->faces[j].box.sizeY, _object->collider->faces[j].box.minZ + _object->collider->faces[j].box.sizeZ };

        collideObject->mesh[i].pos = startPos;
        collideObject->mesh[i + 1].pos = startPos;
        collideObject->mesh[i + 1].pos.x = endPos.x;

        collideObject->mesh[i + 2].pos = startPos;
        collideObject->mesh[i + 2].pos.x = endPos.x;
        collideObject->mesh[i + 3].pos = startPos;
        collideObject->mesh[i + 3].pos.x = endPos.x;
        collideObject->mesh[i + 3].pos.y = endPos.y;

        collideObject->mesh[i + 4].pos = startPos;
        collideObject->mesh[i + 4].pos.x = endPos.x;
        collideObject->mesh[i + 4].pos.y = endPos.y;
        collideObject->mesh[i + 5].pos = startPos;
        collideObject->mesh[i + 5].pos.y = endPos.y;

        collideObject->mesh[i + 6].pos = startPos;
        collideObject->mesh[i + 6].pos.y = endPos.y;
        collideObject->mesh[i + 7].pos = startPos;

        collideObject->mesh[i + 8].pos = startPos;
        collideObject->mesh[i + 8].pos.z = endPos.z;
        collideObject->mesh[i + 9].pos = startPos;
        collideObject->mesh[i + 9].pos.x = endPos.x;
        collideObject->mesh[i + 9].pos.z = endPos.z;

        collideObject->mesh[i + 10].pos = startPos;
        collideObject->mesh[i + 10].pos.x = endPos.x;
        collideObject->mesh[i + 10].pos.z = endPos.z;
        collideObject->mesh[i + 11].pos = endPos;

        collideObject->mesh[i + 12].pos = endPos;
        collideObject->mesh[i + 13].pos = startPos;
        collideObject->mesh[i + 13].pos.y = endPos.y;
        collideObject->mesh[i + 13].pos.z = endPos.z;

        collideObject->mesh[i + 14].pos = startPos;
        collideObject->mesh[i + 14].pos.y = endPos.y;
        collideObject->mesh[i + 14].pos.z = endPos.z;
        collideObject->mesh[i + 15].pos = startPos;
        collideObject->mesh[i + 15].pos.z = endPos.z;

        collideObject->mesh[i + 16].pos = startPos;
        collideObject->mesh[i + 17].pos = startPos;
        collideObject->mesh[i + 17].pos.z = endPos.z;

        collideObject->mesh[i + 18].pos = startPos;
        collideObject->mesh[i + 18].pos.y = endPos.y;
        collideObject->mesh[i + 19].pos = startPos;
        collideObject->mesh[i + 19].pos.y = endPos.y;
        collideObject->mesh[i + 19].pos.z = endPos.z;

        collideObject->mesh[i + 20].pos = startPos;
        collideObject->mesh[i + 20].pos.x = endPos.x;
        collideObject->mesh[i + 21].pos = startPos;
        collideObject->mesh[i + 21].pos.x = endPos.x;
        collideObject->mesh[i + 21].pos.z = endPos.z;

        collideObject->mesh[i + 22].pos = startPos;
        collideObject->mesh[i + 22].pos.x = endPos.x;
        collideObject->mesh[i + 22].pos.y = endPos.y;
        collideObject->mesh[i + 23].pos = endPos;

        i += 24;
    }

    sfVector3f startPos = (sfVector3f){
        _object->collider->box.minX,
        _object->collider->box.minY,
        _object->collider->box.minZ };
    sfVector3f endPos = (sfVector3f){
        _object->collider->box.minX + _object->collider->box.sizeX,
        _object->collider->box.minY + _object->collider->box.sizeY,
        _object->collider->box.minZ + _object->collider->box.sizeZ };

    collideObject->mesh[i].pos = startPos;
    collideObject->mesh[i + 1].pos = startPos;
    collideObject->mesh[i + 1].pos.x = endPos.x;

    collideObject->mesh[i + 2].pos = startPos;
    collideObject->mesh[i + 2].pos.x = endPos.x;
    collideObject->mesh[i + 3].pos = startPos;
    collideObject->mesh[i + 3].pos.x = endPos.x;
    collideObject->mesh[i + 3].pos.y = endPos.y;

    collideObject->mesh[i + 4].pos = startPos;
    collideObject->mesh[i + 4].pos.x = endPos.x;
    collideObject->mesh[i + 4].pos.y = endPos.y;
    collideObject->mesh[i + 5].pos = startPos;
    collideObject->mesh[i + 5].pos.y = endPos.y;

    collideObject->mesh[i + 6].pos = startPos;
    collideObject->mesh[i + 6].pos.y = endPos.y;
    collideObject->mesh[i + 7].pos = startPos;

    collideObject->mesh[i + 8].pos = startPos;
    collideObject->mesh[i + 8].pos.z = endPos.z;
    collideObject->mesh[i + 9].pos = startPos;
    collideObject->mesh[i + 9].pos.x = endPos.x;
    collideObject->mesh[i + 9].pos.z = endPos.z;

    collideObject->mesh[i + 10].pos = startPos;
    collideObject->mesh[i + 10].pos.x = endPos.x;
    collideObject->mesh[i + 10].pos.z = endPos.z;
    collideObject->mesh[i + 11].pos = endPos;

    collideObject->mesh[i + 12].pos = endPos;
    collideObject->mesh[i + 13].pos = startPos;
    collideObject->mesh[i + 13].pos.y = endPos.y;
    collideObject->mesh[i + 13].pos.z = endPos.z;

    collideObject->mesh[i + 14].pos = startPos;
    collideObject->mesh[i + 14].pos.y = endPos.y;
    collideObject->mesh[i + 14].pos.z = endPos.z;
    collideObject->mesh[i + 15].pos = startPos;
    collideObject->mesh[i + 15].pos.z = endPos.z;

    collideObject->mesh[i + 16].pos = startPos;
    collideObject->mesh[i + 17].pos = startPos;
    collideObject->mesh[i + 17].pos.z = endPos.z;

    collideObject->mesh[i + 18].pos = startPos;
    collideObject->mesh[i + 18].pos.y = endPos.y;
    collideObject->mesh[i + 19].pos = startPos;
    collideObject->mesh[i + 19].pos.y = endPos.y;
    collideObject->mesh[i + 19].pos.z = endPos.z;

    collideObject->mesh[i + 20].pos = startPos;
    collideObject->mesh[i + 20].pos.x = endPos.x;
    collideObject->mesh[i + 21].pos = startPos;
    collideObject->mesh[i + 21].pos.x = endPos.x;
    collideObject->mesh[i + 21].pos.z = endPos.z;

    collideObject->mesh[i + 22].pos = startPos;
    collideObject->mesh[i + 22].pos.x = endPos.x;
    collideObject->mesh[i + 22].pos.y = endPos.y;
    collideObject->mesh[i + 23].pos = endPos;
    */
}

void creatMesh(Object3D* _object, char _hasTrueCollider)
{
    glGenVertexArrays(1, &_object->vao);

    glGenBuffers(1, &_object->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _object->vbo);

    updateMesh(_object);

    glBindVertexArray(_object->vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texPos));

    glBindVertexArray(0);

    if (_hasTrueCollider) {
        CollideMesh* collider = creatCollider();

        _object->collider = collider;

        collider->nbFace = _object->nbVertex / 3;
        collider->faces = calloc(collider->nbFace, sizeof(CollideFace));

        updateCollider(_object);

        if (_hasTrueCollider == 2) {
            Object3D* collideObject = creatNewObject(0, 0, 0, (_object->nbVertex / 3) * 2, GL_LINES, 0, shaderIDBasic, 0);
            _object->colliderVisual = collideObject;

            updateCollideVisual(_object);

            creatMesh(collideObject, 0);
        }
    }
}

void setShader(Object3D* _object, GLuint _shaderID, ShaderParam _shaderParam)
{
    _object->shader.ID = _shaderID;

    if (_shaderID != 0) {
        _object->shader.locModel = glGetUniformLocation(_shaderID, "uModel");
        _object->shader.locView = glGetUniformLocation(_shaderID, "uView");
        _object->shader.locProj = glGetUniformLocation(_shaderID, "uProj");
    }

    if (_shaderParam & SHADER_P_LIGTHS) {

        _object->shader.LocNbLigth = glGetUniformLocation(_shaderID, "nbLight");
    }
}

void updateLigth()
{
    Object3D* current = firstObject3D;
    while (current != NULL) {
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Ligth) * nbLigths, ligths);

        current = current->next;
    }
}

void updateMesh(Object3D* _object)
{
    glBindVertexArray(_object->vao);
    glBindBuffer(GL_ARRAY_BUFFER, _object->vbo);
    glBufferData(GL_ARRAY_BUFFER, _object->nbVertex * sizeof(Vertex), _object->mesh, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void object3D_init()
{
    glViewport(0, 0, 1920., 1080.);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    proj = perspective(
        90.f,
        1920.f / 1080.f,
        0.1f,
        1000.f
    );

    //glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.2f, 0.2f, 0.25f, 1.f);

    shaderBasic = sfShader_createFromFile("..\\Ressource\\Basic\\ambient.vert", NULL, "..\\Ressource\\Basic\\vert.frag");
    shaderIDBasic = sfShader_getNativeHandle(shaderBasic);

#if DEBUG & 1
    viewCollisions = creatNewObject(0, 0, 0, 0, GL_LINE_STRIP, 0, shaderIDBasic, 0);

    viewCollisions->nbVertex = 17;
    viewCollisions->mesh = calloc(viewCollisions->nbVertex, sizeof(Vertex));

    creatMesh(viewCollisions, 0);
#endif
}

void object3D_render()
{
    Mat4 model;
    Mat4 view = viewMatrix(&camera);
    Object3D* current = firstObject3D;
    while (current != NULL) {
        model = identity(current);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, current->texture);
        
        glUseProgram(current->shader.ID);

        glUniformMatrix4fv(current->shader.locModel, 1, GL_FALSE, model.m);
        glUniformMatrix4fv(current->shader.locView, 1, GL_FALSE, view.m);
        glUniformMatrix4fv(current->shader.locProj, 1, GL_FALSE, proj.m);
        

        glBindVertexArray(current->vao);

        glDrawArrays(current->renderType, 0, current->nbVertex);

        current = current->next;
    }

#ifdef DEBUG
    for (int i = 0; i < 17; i++) {
        viewCollisions->mesh[i].pos = (sfVector3f){ 0., 0., 0. };
    }

    updateMesh(viewCollisions);
#endif
}

char vecInBoundingBox(sfVector3f _from, sfVector3f _to, BoundingBox3D* _box) {
    return !(_from.x < _box->minX && _to.x < _box->minX 
        || _from.y < _box->minY && _to.y < _box->minY 
        || _from.z < _box->minZ && _to.z < _box->minZ
        || _from.x > _box->minX + _box->sizeX && _to.x > _box->minX + _box->sizeX
        || _from.y > _box->minY + _box->sizeY && _to.y > _box->minY + _box->sizeY
        || _from.z > _box->minZ + _box->sizeZ && _to.z > _box->minZ + _box->sizeZ);
}

char segmentCollision(sfVector3f _from, sfVector3f _move, CollideFace* _face, float* _t) {
    float denominateur = DOT(_move, _face->normal);
    if (denominateur >= 0.) return 0;
    sfVector3f posAO = (sfVector3f){ _face->points[0].x - _from.x, _face->points[0].y - _from.y, _face->points[0].z - _from.z };
    float t = DOT(posAO, _face->normal) / denominateur;
    if (t > 1.) return 1;

    return 1;
}

sfVector3f getMouveVecCollid(sfVector3f _from, sfVector3f _move, unsigned char depth)
{
    CollideMesh* colliderTouch = NULL;
    CollideFace* faceTouch = NULL;
    Object3D* current = firstObject3D;
    sfVector3f to = addVec3(_from, _move);
    while (current != NULL) {
        if (current->collider) {
            if (vecInBoundingBox(_from, to, &current->collider->box)) {
                colliderTouch = current->collider;

                for (int i = 0; i < colliderTouch->nbFace; i++) {
                    if (vecInBoundingBox(_from, to, &colliderTouch->faces[i].box)) {
                        float t;
                        if (segmentCollision(_from, _move, &colliderTouch->faces[i], &t)) {
                            to = _from;
                            faceTouch = &colliderTouch->faces[i];
                        }
                    }
                }
            }
        }

        current = current->next;
    }

#ifdef DEBUG

    if (faceTouch) {
        sfVector3f startPos = (sfVector3f){ faceTouch->box.minX, faceTouch->box.minY, faceTouch->box.minZ };
        sfVector3f endPos = (sfVector3f){ faceTouch->box.minX + faceTouch->box.sizeX, faceTouch->box.minY + faceTouch->box.sizeY, faceTouch->box.minZ + faceTouch->box.sizeZ };

        viewCollisions->mesh[0].pos = (sfVector3f){ startPos.x, startPos.y, startPos.z };
        viewCollisions->mesh[1].pos = (sfVector3f){ startPos.x, endPos.y, startPos.z };
        viewCollisions->mesh[2].pos = (sfVector3f){ endPos.x, endPos.y, startPos.z };
        viewCollisions->mesh[3].pos = (sfVector3f){ endPos.x, startPos.y, startPos.z };
        viewCollisions->mesh[4].pos = (sfVector3f){ endPos.x, endPos.y, startPos.z };
        viewCollisions->mesh[5].pos = (sfVector3f){ endPos.x, endPos.y, endPos.z };
        viewCollisions->mesh[6].pos = (sfVector3f){ endPos.x, startPos.y, endPos.z };
        viewCollisions->mesh[7].pos = (sfVector3f){ endPos.x, endPos.y, endPos.z };
        viewCollisions->mesh[8].pos = (sfVector3f){ startPos.x, endPos.y, endPos.z };
        viewCollisions->mesh[9].pos = (sfVector3f){ startPos.x, startPos.y, endPos.z };
        viewCollisions->mesh[10].pos = (sfVector3f){ startPos.x, endPos.y, endPos.z };
        viewCollisions->mesh[11].pos = (sfVector3f){ startPos.x, endPos.y, startPos.z };
        viewCollisions->mesh[12].pos = (sfVector3f){ startPos.x, startPos.y, startPos.z };
        viewCollisions->mesh[13].pos = (sfVector3f){ endPos.x, startPos.y, startPos.z };
        viewCollisions->mesh[14].pos = (sfVector3f){ endPos.x, startPos.y, endPos.z };
        viewCollisions->mesh[15].pos = (sfVector3f){ startPos.x, startPos.y, endPos.z };
        viewCollisions->mesh[16].pos = (sfVector3f){ startPos.x, startPos.y, startPos.z };

        updateMesh(viewCollisions);
    }
#endif // DEBUG

    return to;
}

Camera* getCam()
{
    return &camera;
}

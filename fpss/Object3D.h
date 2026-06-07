#pragma once
#include <stdio.h>
#include <glad/glad.h>
#include <SFML/OpenGL.h>
#include <SFML/Window.h>
#include <SFML/Graphics.h>

#include <math.h>

#ifdef _DEBUG
#define DEBUG 1
#endif // _DEBUG

#define RAD_TO_DEG (180 / 3.14)
#define DEG_TO_RAD (3.14 / 180)

#define DOT(v1,v2) (v1.x*v2.x + v1.y*v2.y+v1.z*v2.z)
#define CROSS(rez,v1,v2) \
rez.x = v1.y*v2.z - v1.z*v2.y; \
rez.y = v1.z*v2.x - v1.x*v2.z; \
rez.z = v1.x*v2.y - v1.y*v2.x;
#define SUB(rez,v1,v2) \
rez.x = v1.x-v2.x; \
rez.y = v1.y-v2.y; \
rez.z = v1.z-v2.z;
#define LENGTH(v) (sqrtf(v.x* v.x + v.y*v.y + v.z*v.z))

typedef struct BoundingBox3D {
    float minX, minY, minZ;
    float sizeX, sizeY, sizeZ;
} BoundingBox3D;

typedef struct CollideFace {
    sfVector3f points[3];
    sfVector3f normal;

    BoundingBox3D box;
} CollideFace;

typedef struct CollideMesh CollideMesh;
struct CollideMesh {
    CollideMesh* next;
    unsigned int nbFace;
    CollideFace* faces;

    BoundingBox3D box;
};

typedef char ShaderParam;
enum ShaderParam {
    SHADER_P_LIGTHS = 0x01,
};

typedef struct Mat4 {
    float m[16];
} Mat4;

typedef struct ShaderComp {
    GLuint ID;

    unsigned char shaderParam;

    GLuint locModel, locView, locProj;

    GLuint LocNbLigth;
} ShaderComp;

typedef struct Color {
    float r;
    float g;
    float b;
    float a;
} Color; 

typedef struct
{
    sfVector3f pos;
    sfVector3f norm;
    sfVector2f texPos;
} Vertex;

typedef struct FaceCorner {
    unsigned int posIndex;
    unsigned int textIndex;
    unsigned int normalIndex;
} FaceCorner;

typedef struct Face {
    FaceCorner corners[3];
} Face;

typedef struct Object3D Object3D;
struct Object3D {
    Object3D* next;
    GLuint texture;
    sfVector3f* poss;
    sfVector2f* texs;
    sfVector3f* normals;
    unsigned int nbFaces;
    Face* faces;
    sfVector3f pos;
    float yaw, pitch, roll;

    GLuint vao;
    GLuint vbo;
    unsigned int nbVertex;
    Vertex* mesh;
    GLuint renderType;

    CollideMesh* collider;
    Object3D* colliderVisual;

    ShaderComp shader;
};

typedef struct Camera
{
    float x;
    float y;
    float z;

    float yaw;
    float pitch;
} Camera;

typedef struct Ligth {
    sfVector3f pos;
    Color color;
    float power;
} Ligth;

sfVector3f computeNormal(sfVector3f a, sfVector3f b, sfVector3f c);

void readText(FILE* _file, char _link[256]);

void eventCamera(sfEvent* event, sfRenderWindow* window);
void updateCamera(float dt);

Ligth* createLigth(Color _color, sfVector3f _pos, float _power);
CollideMesh* creatCollider();
Object3D* loadModel(const char* _link, GLuint _renderType, float _scale, GLuint _shader, ShaderParam _shaderParam, char _hasTrueCollider);
Object3D* creatNewObject(unsigned int nbPos, unsigned int nbText, unsigned int nbNormals, unsigned int nbFaces, GLuint _renderType, GLuint texture, GLuint _shaderID, ShaderParam _shaderParam);
void creatMesh(Object3D* _object, char _hasTrueCollider);
void setShader(Object3D* _object, GLuint _shaderID, ShaderParam _shaderParam);

Mat4 perspective(float _fov, float _aspect, float _near, float _far); 
Mat4 viewMatrix(Camera* _camera);
Mat4 identity(Object3D* _model);

void updateCollider(Object3D* _object);
void updateCollideVisual(Object3D* _object);
void updateLigth();
void updateMesh(Object3D* _object);

void object3D_init();
void object3D_render();

char segmentCollision(sfVector3f _from, sfVector3f _move, CollideFace* _face);

sfVector3f getMouveVecCollid(sfVector3f _from, sfVector3f _move, unsigned char depth);

Camera* getCam();

sfVector3f mulV3f(sfVector3f _v1, float _multi);
sfVector3f addVec3(sfVector3f _a, sfVector3f _b);

void object3D_destroy(Object3D* _object);

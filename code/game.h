#include "voxel.h"

struct Entity
{
    Vector3f myPosition;
    Vector3f myVelocity;
};

struct CameraEntity
{
    Vector3f myParentOffset;
    Entity* myParentEntity;
};

struct GameState
{
    World myWorld;
    
    Entity myPlayer;
    CameraEntity myCameraEntity;
    
    bool myUseDebugCamera = false;
};
static GameState ourGameState;
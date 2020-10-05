#include "voxel.h"

struct GameState
{
    World myWorld;
    
    Vector3f myPlayerPosition;
    Vector3f myPlayerVelocity;
    
    bool myUseDebugCamera = false;
};
static GameState ourGameState;
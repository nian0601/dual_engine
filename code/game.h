#include "voxel.h"

struct GameState
{
    World myWorld;
    
    Vector3f myPlayerPosition;
    
    bool myUseDebugCamera = false;
};
static GameState ourGameState;
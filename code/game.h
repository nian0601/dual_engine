#define GRIDSIZE 64
#define CELLCOUNT GRIDSIZE * GRIDSIZE
#define CELLSIZE 1

#include "voxel.h"

enum TileType
{
    GRASS,
    SHALLOW_WATER,
    DEEP_WATER,
    DIRT,
    SAND,
    SPAWN_POSITION,
};

struct PathNode
{
    bool operator<(const PathNode& aOther) const { return myTotalCost < aOther.myTotalCost; }
    enum ListState
    {
        IN_OPEN,
        IN_CLOSED,
        NOT_VISITED,
    };
    
    Entity* myEntity;
    PathNode* myParent;
    int myCurrentCost;
    int myHeuristic;
    int myTotalCost;
    
    ListState myState;
};

struct Path
{
    GrowingArray<Entity*> myPoints;
    int myCurrentPoint;
};

struct Map
{
    Entity myTiles[CELLCOUNT];
    TileType myTileTypes[CELLCOUNT];
    
    // Move to separate data-structure if we want to be able to run queries in parallell?
    PathNode myPathNodes[CELLCOUNT];
};


struct GameState
{
    Map myMap;
    
    Entity myPlayer;
    Path myPlayerPath;
    
    Entity* myEntityUnderMouse;
    
    World myWorld;
};
static GameState ourGameState;
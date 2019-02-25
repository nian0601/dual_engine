#define GRIDSIZE 64
#define CELLCOUNT GRIDSIZE * GRIDSIZE
#define CELLSIZE 1

enum TileType
{
    GRASS,
    SHALLOW_WATER,
    DEEP_WATER,
    DIRT,
    SAND,
    SPAWN_POSITION,
};

struct Map
{
    Entity myTiles[GRIDSIZE * GRIDSIZE];
    TileType myTileTypes[GRIDSIZE * GRIDSIZE];
};

struct GameState
{
    Map myMap;
    
    Entity myPlayer;
    Entity* myEntityUnderMouse;
};
static GameState ourGameState;
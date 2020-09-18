#include "FastNoise.h"

enum BlockType
{
    InvalidBlockType,
    Stone,
    Dirt,
    Grass,
    BlockTypeCount,
};

struct BlockInfo
{
    BlockType myBlockType;
};
static BlockInfo ourBlockInfos[BlockTypeCount];


static const int ChunkSize = 32;
struct Chunk
{
    GrowingArray<int> myBlocks;
    Vector3f myWorldPosition;
    int myChunkX;
    int myChunkY;
    int myChunkZ;
    int myMeshID;
};

static const int WorldSize = 8;
static const int WorldHeight = 2;
static const int MaxChunksToCreatePerUpdate = 4;
struct World
{
    GrowingArray<Chunk*> myChunks;
    
    GrowingArray<Chunk*> myChunksToBuild;
    GrowingArray<Chunk*> myChunksToRender;
    
    FastNoise myNoise;
};
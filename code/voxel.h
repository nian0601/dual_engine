#include "FastNoise.h"

enum BlockType
{
    Type0,
    Type1,
    Type2,
    Type3,
    BlockTypeCount,
};

struct BlockInfo
{
    BlockType myBlockType;
};
static BlockInfo ourBlockInfos[BlockTypeCount];


static const int ChunkSize = 16;
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
static const int MaxChunksToCreatePerUpdate = 1;
struct World
{
    GrowingArray<Chunk*> myChunks;
    
    GrowingArray<Chunk*> myChunksToBuild;
    GrowingArray<Chunk*> myChunksToRender;
    
    FastNoise myNoise;
};
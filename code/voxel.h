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


// Keep this power of two to make sure that collisionchecking is nice and easy
static const int ChunkSize = 32;
static const Vector3f ChunkMidOffset = {ChunkSize * 0.5f, ChunkSize * 0.5f, ChunkSize * 0.5f};
struct Chunk
{
    GrowingArray<int> myBlocks;
    Vector3f myWorldPosition;
    int myChunkX;
    int myChunkY;
    int myChunkZ;
    int myMeshID = -1;
};

static const int WorldSize = 5;
static const int WorldHeight = 3;
static const int MaxChunksToCreatePerUpdate = 4;
struct World
{
    GrowingArray<Chunk*> myChunks;
    
    GrowingArray<Chunk*> myChunksToInit;
    GrowingArray<Chunk*> myChunksToBuildMesh;
    GrowingArray<Chunk*> myChunksToRender;
    
    FastNoise myNoise;
};
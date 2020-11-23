#include "FastNoise.h"

enum BlockType
{
    InvalidBlockType,
    Stone,
    Dirt,
    Grass,
    Water,
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
static const Vector3f ChunkExtents = {ChunkSize, ChunkSize, ChunkSize};

struct Chunk
{
    enum ChunkState
    {
        UNINITIALIZED,
        FROZEN,
        ACTIVE,
    };
    
    GrowingArray<int> myBlocks;
    Vector3f myWorldPosition;
    Vector3i myChunkPosition;
    DE_AABB myAABB;
    int myMeshID = -1;
    int myState = UNINITIALIZED;
};

struct BlockRaycastHit
{
    int myX;
    int myY;
    int myZ;
    float myRaycastDistance;
    Vector3f myHitPosition;
};

struct ChunkRaycastHit
{
    Chunk* myChunk;
    BlockRaycastHit myClosestHit;
    GrowingArray<BlockRaycastHit> myBlockHits;
};

struct ChunkSectionAABB
{
    Chunk* myChunk;
    int myX;
    int myY;
    int myZ;
    int mySize;
};

// We'll load+render this many extra chunks around the player
// i.e 1 = a 3x3x3 cube around the player will be loaded (1 additional chunk in all directions)
static const int ChunkStreamingBorderSize = 3;
static const int WorldSize = 15;
static const int WorldHeight = 3;
static const int MaxChunksToCreatePerUpdate = 4;
struct World
{
    GrowingArray<Chunk*> myChunks;

    GrowingArray<Chunk*> myUninitializedChunks;
    GrowingArray<Chunk*> myActiveChunks;
    GrowingArray<Chunk*> myFrozenChunks;
    
    GrowingArray<Chunk*> myChunksToBuildMesh;
    
    FastNoise myNoise;
    
    Vector3i myStreamingCenterChunk;
};
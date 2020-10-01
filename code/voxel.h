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
    GrowingArray<int> myBlocks;
    Vector3f myWorldPosition;
    int myChunkX;
    int myChunkY;
    int myChunkZ;
    int myMeshID = -1;
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

static const int WorldSize = 5;
static const int WorldHeight = 3;
static const int MaxChunksToCreatePerUpdate = 4;
struct World
{
    GrowingArray<Chunk*> myChunks;

    // Once I move the meshbuilding to a separate thread (if I end up doing that)
    // then I need to reorganize these lists. For now each Chunk i splaced both the 'BuildMesh' and 'Render'-lists
    // once it has been Initialized. But to make sure that we never have cases 
    // where a chunk disappears for a frame we never remove them from the 'Render' list.
    // To support threating we'll need some way to keep the old mesh alive 
    // until the new mesh has been built to be able to support this.
    GrowingArray<Chunk*> myChunksToInit;
    GrowingArray<Chunk*> myChunksToBuildMesh;
    GrowingArray<Chunk*> myChunksToRender;
    
    FastNoise myNoise;
};
enum BlockType
{
    Type1,
    Type2,
    Type3,
};

struct Block
{
    BlockType myBlockType;
};


static const int ChunkSize = 16;
struct Chunk
{
    GrowingArray<Block> myBlocks;
    Vector3f myWorldPosition;
    Vector3f myChunkPosition;
    int myMeshID;
};

static const int WorldSize = 6;
static const int MaxChunksToCreatePerUpdate = 1;
struct World
{
    GrowingArray<Chunk*> myChunks;
    
    GrowingArray<Chunk*> myChunksToBuild;
    GrowingArray<Chunk*> myChunksToRender;
};
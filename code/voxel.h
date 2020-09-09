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
    Block*** myBlocks;
    Vector3f myPosition;
    int myMeshID;
};

static const int WorldSize = 4;
struct World
{
    Chunk*** myChunks;
};
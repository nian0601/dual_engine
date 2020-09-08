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
};

static const int WorldSize = 2;
struct World
{
    Chunk*** myChunks;
};
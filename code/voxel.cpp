void FillChunk(Chunk* aChunk, const Vector3f& aPosition)
{
    aChunk->myPosition = aPosition;
    
    aChunk->myBlocks = new Block**[ChunkSize];
    
    for(int x = 0; x < ChunkSize; ++x)
    {
        aChunk->myBlocks[x] = new Block*[ChunkSize];
        for(int y = 0; y < ChunkSize; ++y)
        {
            aChunk->myBlocks[x][y] = new Block[ChunkSize];
            for(int z = 0; z < ChunkSize; ++z)
            {
                Block* block = &aChunk->myBlocks[x][y][z];
                block->myBlockType = static_cast<BlockType>(rand() % 3);
            }
        }
    }
}

void CreateWorld()
{
    ourGameState.myWorld.myChunks = new Chunk**[WorldSize];
    
    for(int x = 0; x < WorldSize; x++)
    {
        ourGameState.myWorld.myChunks[x] = new Chunk*[WorldSize];
        for(int y = 0; y < WorldSize; ++y)
        {
            ourGameState.myWorld.myChunks[x][y] = new Chunk[ChunkSize];
            for(int z = 0; z < WorldSize; ++z)
            {
                Vector3f chunkPosition;
                chunkPosition.x = float(x) * (ChunkSize + 1);
                chunkPosition.y = float(y) * (ChunkSize + 1);
                chunkPosition.z = float(z) * (ChunkSize + 1);
                
                FillChunk(&ourGameState.myWorld.myChunks[x][y][z], chunkPosition);
            }
        }
    }
}

void RenderChunk(Chunk* aChunk)
{
    for(int x = 0; x < ChunkSize; ++x)
    {
        for(int y = 0; y < ChunkSize; ++y)
        {
            for(int z = 0; z < ChunkSize; ++z)
            {
                Block* block = &aChunk->myBlocks[x][y][z];
                
                Vector4f color;
                switch(block->myBlockType)
                {
                    case Type1: color = {0.54f, 0.57f, 0.54f, 1.f}; break;
                    case Type2: color = {0.54f, 0.54f, 0.51f, 1.f}; break;
                    case Type3: color = {0.55f, 0.51f, 0.52f, 1.f}; break;
                }
                
                Vector3f blockposition = {float(x), float(y), float(z)};
                QueueCube(aChunk->myPosition + blockposition, {1.f, 1.f, 1.f}, color);
            }
        }
    }
}

void RenderWorld()
{
    for(int x = 0; x < WorldSize; x++)
    {
        for(int y = 0; y < WorldSize; ++y)
        {
            for(int z = 0; z < WorldSize; ++z)
            {
                RenderChunk(&ourGameState.myWorld.myChunks[x][y][z]);
            }
        }
    }
}
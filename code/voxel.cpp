void FillChunk(Chunk* aChunk)
{
    ArrayAlloc(aChunk->myBlocks, ChunkSize * ChunkSize * ChunkSize);
    
    for(int x = 0; x < ChunkSize; ++x)
    {
        for(int y = 0; y < ChunkSize; ++y)
        {
            for(int z = 0; z < ChunkSize; ++z)
            {
                int voxelX = aChunk->myChunkX * ChunkSize + x;
                int voxelZ = aChunk->myChunkZ * ChunkSize + z;
                
                float value = ourGameState.myWorld.myNoise.GetValue(voxelX, voxelZ);
                value += 1.f;
                value *= 0.5f;
                
                value *= 15.f;
                
                int voxelY = aChunk->myChunkY * ChunkSize + y;
                
                int blockValue = 2;
                if(voxelY > value)
                    blockValue = 6;
                else
                {
                    if(voxelY < 5)
                        blockValue = 1;
                    else if(voxelY < 10)
                        blockValue = 2;
                    else if(voxelY < 15)
                        blockValue = 3;
                }
                
                ArrayAdd(aChunk->myBlocks, blockValue);
            }
        }
    }
}

void BuildChunkMesh(Chunk* aChunk)
{
    aChunk->myMeshID = gfx_CreateMesh();
    
    for(int x = 0; x < ChunkSize; ++x)
    {
        for(int y = 0; y < ChunkSize; ++y)
        {
            for(int z = 0; z < ChunkSize; ++z)
            {
                int index = (x * ChunkSize * ChunkSize) + (y * ChunkSize) + z;
                
                int blockType = aChunk->myBlocks[index];
                if(blockType == 6)
                    continue;
                
                Vector4f color;
                switch(blockType)
                {
                    //case Type0:
                    //case Type1: color = {0.54f, 0.57f, 0.54f, 1.f}; break;
                    //case Type2: color = {0.54f, 0.54f, 0.51f, 1.f}; break;
                    //case Type3: color = {0.55f, 0.51f, 0.52f, 1.f}; break;
                    
                    //case Type0:
                    //case Type1: color = {1.0f, 0.0f, 0.0f, 1.f}; break;
                    //case Type2: color = {0.0f, 1.0f, 0.0f, 1.f}; break;
                    //case Type3: color = {0.0f, 0.0f, 1.0f, 1.f}; break;
                    
                    case Type0: color = {0.0f, 0.5f, 0.0f, 1.f}; break;
                    case Type1: color = {0.0f, 0.8f, 0.0f, 1.f}; break;
                    case Type2: color = {0.0f, 0.75f, 0.0f, 1.f}; break;
                    case Type3: color = {0.0f, 0.7f, 0.0f, 1.f}; break;
                }
                
                gfx_CreateCubeMesh(aChunk->myMeshID, float(x), float(y), float(z), color.x, color.y, color.z);
            }
        }
    }
    
    gfx_FinishMesh(aChunk->myMeshID);
}

void CreateWorld()
{
    ArrayAlloc(ourGameState.myWorld.myChunks, WorldSize * WorldSize * WorldSize);
    ArrayAlloc(ourGameState.myWorld.myChunksToBuild, WorldSize);
    ArrayAlloc(ourGameState.myWorld.myChunksToRender, WorldSize);

    for(int x = 0; x < WorldSize; ++x)
    {
        for(int y = 0; y < WorldSize; ++y)
        {
            for(int z = 0; z < WorldSize; z++)
            {
                Chunk* chunk = new Chunk();
                chunk->myChunkX = x;
                chunk->myChunkY = y;
                chunk->myChunkZ = z;
                chunk->myWorldPosition.x = x * ChunkSize;
                chunk->myWorldPosition.y = y * ChunkSize;
                chunk->myWorldPosition.z = z * ChunkSize;
                
                ArrayAdd(ourGameState.myWorld.myChunks, chunk);
                ArrayAdd(ourGameState.myWorld.myChunksToBuild, chunk);
            }
        }
    }
}

void RenderChunk(Chunk* aChunk)
{
    QueueMesh(aChunk->myMeshID, aChunk->myWorldPosition);
}

void RenderWorld()
{
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myChunksToRender;
    for(int i = 0; i < chunks.myCount; ++i)
        RenderChunk(chunks[i]);
}

void BuildChunks()
{
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myChunksToBuild;
    int chunksRemoved = 0;
    for(int i = 0; i < chunks.myCount && chunksRemoved < MaxChunksToCreatePerUpdate;)
    {
        Chunk* chunk = chunks[i];
        ArrayRemoveCyclic(chunks, i);
        FillChunk(chunk);
        BuildChunkMesh(chunk);
        
        ArrayAdd(ourGameState.myWorld.myChunksToRender, chunk);
        
        ++chunksRemoved;
    }
}

void UpdateWorld()
{
    BuildChunks();
}
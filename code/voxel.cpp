int GetBlockIndex(int x, int y, int z)
{
    return (x * ChunkSize * ChunkSize) + (y * ChunkSize) + z;
}

int GetBlockType(Chunk* aChunk, int x, int y, int z)
{
    ASSERT(x >= 0 && x < ChunkSize);
    ASSERT(y >= 0 && y < ChunkSize);
    ASSERT(z >= 0 && z < ChunkSize);
    
    return aChunk->myBlocks[GetBlockIndex(x, y, z)];
}

void FillChunk(Chunk* aChunk)
{
    ArrayAlloc(aChunk->myBlocks, ChunkSize * ChunkSize * ChunkSize);
    
    int grassThickness = 1;
    int dirtThickness = 3;
    for(int x = 0; x < ChunkSize; ++x)
    {
        int voxelX = aChunk->myChunkX * ChunkSize + x;
        
        for(int y = 0; y < ChunkSize; ++y)
        {
            int voxelY = aChunk->myChunkY * ChunkSize + y;
            
            for(int z = 0; z < ChunkSize; ++z)
            {
                int voxelZ = aChunk->myChunkZ * ChunkSize + z;
                
                float value = ourGameState.myWorld.myNoise.GetSimplex(voxelX, voxelZ);
                value += 1.f;
                value *= 0.5f;
                
                int groundLevel = static_cast<int>(value * 30.f);
                
                
                int blockValue = InvalidBlockType;
                if(voxelY <= groundLevel)
                {
                    if(voxelY == groundLevel)
                    {
                        blockValue = Grass;
                    }
                    else if(voxelY > groundLevel - dirtThickness)
                    {
                        blockValue = Dirt;
                    }
                    else
                    {
                        blockValue = Stone;
                    }
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
                int index = GetBlockIndex(x, y, z);
                int blockType = aChunk->myBlocks[index];
     
                if(blockType == InvalidBlockType)
                    continue;
                
                bool isXEdge = x == 0 || x == ChunkSize - 1;
                bool isYEdge = y == 0 || y == ChunkSize - 1;
                bool isZEdge = z == 0 || z == ChunkSize - 1;
                
                if(!isXEdge && !isYEdge && !isZEdge)
                {   
                    bool fullySurrounded = true;
                    if(GetBlockType(aChunk, x - 1, y, z) != InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x + 1, y, z) != InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y - 1, z) != InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y + 1, z) != InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y, z - 1) != InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y, z + 1) != InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(fullySurrounded)
                        continue;
                }
                
                Vector4f color = {0.f, 0.f, 0.f, 1.f};
                
                switch(blockType)
                {
                    case Grass: color = {0.05f, 0.75f, 0.1f, 1.f}; break;
                    case Dirt: color = {77.f/256.f, 39.f/256.f, 14.f/256.f, 1.f}; break;
                    case Stone: color = {87.f/256.f, 83.f/256.f, 80.f/256.f, 1.f};
                }
                
                gfx_CreateCubeMesh(aChunk->myMeshID, float(x), float(y), float(z), color.x, color.y, color.z);
            }
        }
    }
    
    gfx_FinishMesh(aChunk->myMeshID);
}

void CreateWorld()
{
    ArrayAlloc(ourGameState.myWorld.myChunks, WorldSize * WorldHeight  * WorldSize);
    ArrayAlloc(ourGameState.myWorld.myChunksToBuild, WorldSize);
    ArrayAlloc(ourGameState.myWorld.myChunksToRender, WorldSize);

    #if 1
    for(int x = 0; x < WorldSize; ++x)
    {
        for(int y = 0; y < WorldHeight; ++y)
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
    #else
    int startX = 1;
    int startY = 0;
    int startZ = 1;
    int endX = 3;
    int endY = 1;
    int endZ = 3;
    
    for(int x = startX; x < endX; ++x)
    {
        for(int y = startY; y < endY; ++y)
        {
            for(int z = startZ; z < endZ; z++)
            {
                Chunk* chunk = new Chunk();
                chunk->myChunkX = x;
                chunk->myChunkY = y;
                chunk->myChunkZ = z;
                chunk->myWorldPosition.x = x * ChunkSize + (x * 5.f);
                chunk->myWorldPosition.y = y * ChunkSize;
                chunk->myWorldPosition.z = z * ChunkSize + (z * 5.f);
                
                ArrayAdd(ourGameState.myWorld.myChunks, chunk);
                ArrayAdd(ourGameState.myWorld.myChunksToBuild, chunk);
            }
        }
    }
    #endif

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
    while(chunks.myCount > 0 && chunksRemoved < MaxChunksToCreatePerUpdate)
    {
        Chunk* chunk = chunks[chunks.myCount - 1];
        ArrayRemoveCyclic(chunks, chunks.myCount - 1);
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
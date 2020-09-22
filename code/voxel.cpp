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

Vector3f GetBlockCenterWorldPosition(Chunk* aChunk, int x, int y, int z)
{
    Vector3f position;
    position.x = aChunk->myWorldPosition.x + x + 0.5f;
    position.y = aChunk->myWorldPosition.y + y + 0.5f;
    position.z = aChunk->myWorldPosition.z + z + 0.5f;
    return position;
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
    if(aChunk->myMeshID == -1)
        aChunk->myMeshID = gfx_CreateMesh();
    
    gfx_ClearMesh(aChunk->myMeshID);
    
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
    ArrayAlloc(ourGameState.myWorld.myChunksToInit, WorldSize);
    ArrayAlloc(ourGameState.myWorld.myChunksToBuildMesh, WorldSize);
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
                chunk->myMeshID = -1;
                
                ArrayAdd(ourGameState.myWorld.myChunks, chunk);
                ArrayAdd(ourGameState.myWorld.myChunksToInit, chunk);
            }
        }
    }
#else
    int startX = 0;
    int startY = 0;
    int startZ = 0;
    int endX = 2;
    int endY = 1;
    int endZ = 2;
    
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
                chunk->myMeshID = -1;
                
                ArrayAdd(ourGameState.myWorld.myChunks, chunk);
                ArrayAdd(ourGameState.myWorld.myChunksToInit, chunk);
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

void InitChunks()
{
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myChunksToInit;
    int chunksRemoved = 0;
    while(chunks.myCount > 0 && chunksRemoved < MaxChunksToCreatePerUpdate)
    {
        Chunk* chunk = chunks[chunks.myCount - 1];
        ArrayRemoveCyclic(chunks, chunks.myCount - 1);
        
        FillChunk(chunk);
        
        ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, chunk);
        
        ++chunksRemoved;
    }
}

void BuildChunkMeshes()
{
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myChunksToBuildMesh;
    int chunksRemoved = 0;
    while(chunks.myCount > 0 && chunksRemoved < MaxChunksToCreatePerUpdate)
    {
        Chunk* chunk = chunks[chunks.myCount - 1];
        ArrayRemoveCyclic(chunks, chunks.myCount - 1);
        
        BuildChunkMesh(chunk);
        
        ArrayAdd(ourGameState.myWorld.myChunksToRender, chunk);
        
        ++chunksRemoved;
    }
}

void UpdateWorld()
{
    InitChunks();
    BuildChunkMeshes();
}

void ModifyBlocksInSphere(Chunk* aChunk, int aNewBlockType, const Vector3f& aPosition, float aRadius)
{
    Vector3f chunkCenter = aChunk->myWorldPosition + ChunkMidOffset;
    
    float blockSphereRadius = (aRadius + 0.5f) * (aRadius + 0.5f);
    for(int x = 0; x < ChunkSize; ++x)
    {
        for(int y = 0; y < ChunkSize; ++y)
        {
            for(int z = 0; z < ChunkSize; ++z)
            {
                Vector3f blockCenterPosition = GetBlockCenterWorldPosition(aChunk, x, y, z);
                
                float distToBlock = Length2(aPosition - blockCenterPosition);
                if(distToBlock <= blockSphereRadius)
                {
                    int blockIndex = GetBlockIndex(x, y, z);
                    aChunk->myBlocks[blockIndex] = aNewBlockType;
                }
            }
        }
    }
    
    ArrayRemoveCyclic(ourGameState.myWorld.myChunksToRender, aChunk);
    ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, aChunk);
}

void RemoveBlocksInSphere(const Vector3f& aPosition, float aRadius)
{
    for(int i = 0; i < ourGameState.myWorld.myChunksToRender.myCount;)
    {
        Chunk* chunk = ourGameState.myWorld.myChunksToRender[i];
        
        Vector3f chunkCenter = chunk->myWorldPosition + ChunkMidOffset;
        float distance = Length2(aPosition - chunkCenter);
        
        float combinedRadius = aRadius + (ChunkSize * 0.5f);
        combinedRadius *= combinedRadius;
        
        if(distance <= combinedRadius)
        {
            ModifyBlocksInSphere(chunk, InvalidBlockType, aPosition, aRadius);
        }
        else
        {
            ++i;
        }
    }
}
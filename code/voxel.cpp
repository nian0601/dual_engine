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
    if(aChunk->myMeshID != -1)
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
        ArrayAdd(ourGameState.myWorld.myChunksToRender, chunk);
        
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
    bool wasModified = false;
    
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
                    
                    if(aChunk->myBlocks[blockIndex] != aNewBlockType)
                    {
                        aChunk->myBlocks[blockIndex] = aNewBlockType;
                        wasModified = true;
                    }
                    
                }
            }
        }
    }
    
    if(wasModified)
    {
        ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, aChunk);
    }
}

void ModifyBlocksInSphere(const Vector3f& aPosition, float aRadius, int aNewBlockType)
{
    for(int i = 0; i < ourGameState.myWorld.myChunksToRender.myCount; ++i)
    {
        Chunk* chunk = ourGameState.myWorld.myChunksToRender[i];
        
        Vector3f chunkCenter = chunk->myWorldPosition + ChunkMidOffset;
        float distance = Length2(aPosition - chunkCenter);
        
        float combinedRadius = aRadius + (ChunkSize * 0.5f);
        combinedRadius *= combinedRadius;
        
        bool chunkWasModified = false;
        if(distance <= combinedRadius)
            ModifyBlocksInSphere(chunk, aNewBlockType, aPosition, aRadius);
    }
}

void DoRaycast(const DE_Ray& aMouseRay, const ChunkSectionAABB& aSection, ChunkRaycastHit& outHits)
{
    Vector3f minPos = aSection.myChunk->myWorldPosition;
    minPos.x += aSection.myX;
    minPos.y += aSection.myY;
    minPos.z += aSection.myZ;
    
    Vector3f maxPos = minPos;
    maxPos.x += aSection.mySize;
    maxPos.y += aSection.mySize;
    maxPos.z += aSection.mySize;
    
    DE_AABB aabb = AABBFromPoints(minPos, maxPos);
    float distance = LineVSAABB(aabb, aMouseRay, NULL);
    
    if(distance > -1.f)
    {
        if(aSection.mySize > 4)
        {
            int newSize = aSection.mySize / 2;
            
            ChunkSectionAABB section;
            section.myChunk = aSection.myChunk;
            section.mySize = newSize;
            
            for(int x = 0; x < 2; ++x)
            {
                for(int y = 0; y < 2; ++y)
                {
                    for(int z = 0; z < 2; ++z)
                    {
                        section.myX = aSection.myX + (newSize * x);
                        section.myY = aSection.myY + (newSize * y);
                        section.myZ = aSection.myZ + (newSize * z);
                        
                        DoRaycast(aMouseRay, section, outHits);
                    }
                }
            }
        }
        else
        {
            int endX = aSection.myX + aSection.mySize;
            for(int x = aSection.myX; x < endX; ++x)
            {
                int endY = aSection.myY + aSection.mySize;
                for(int y = aSection.myY; y < endY; ++y)
                {
                    int endZ = aSection.myZ + aSection.mySize;
                    for(int z = aSection.myZ; z < endZ; ++z)
                    {
                        BlockRaycastHit& hit = ArrayAdd(outHits.myBlockHits);
                        hit.myX = x;
                        hit.myY = y;
                        hit.myZ = z;
                    }
                }
            }
        }
    }
}

void ModifyBlockUnderMouse(const DE_Ray& aMouseRay, int aNewBlockType)
{
    GrowingArray<ChunkRaycastHit> raycastHits;
    ArrayAlloc(raycastHits, 16);
    
    for(int i = 0; i < ourGameState.myWorld.myChunksToRender.myCount; ++i)
    {
        Chunk* chunk = ourGameState.myWorld.myChunksToRender[i];
        Vector3f minPos = chunk->myWorldPosition;
        Vector3f maxPos = minPos + ChunkExtents;
        
        ChunkSectionAABB section;
        section.myChunk = chunk;
        section.myX = 0;
        section.myY = 0;
        section.myZ = 0;
        section.mySize = ChunkSize;
        
        ChunkRaycastHit& chunkHit = ArrayAdd(raycastHits);
        chunkHit.myChunk = chunk;
        ArrayAlloc(chunkHit.myBlockHits, 16);
        DoRaycast(aMouseRay, section, chunkHit);
    }
    
    for(int i = 0; i < raycastHits.myCount; ++i)
    {
        ChunkRaycastHit& chunkHit = raycastHits[i];
        
        bool wasEdited = false;
        for(int j = 0; j < chunkHit.myBlockHits.myCount; ++j)
        {
            BlockRaycastHit& hit = chunkHit.myBlockHits[j];
            int blockIndex = GetBlockIndex(hit.myX, hit.myY, hit.myZ);
            if(chunkHit.myChunk->myBlocks[blockIndex] != aNewBlockType)
            {
                chunkHit.myChunk->myBlocks[blockIndex] = aNewBlockType;
                wasEdited = true;
            }
        }
        
        if(wasEdited)
            ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, chunkHit.myChunk);
    }
}
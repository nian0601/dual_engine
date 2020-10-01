int GetBlockIndex(int x, int y, int z)
{
    ASSERT(x >= 0 && x < ChunkSize);
    ASSERT(y >= 0 && y < ChunkSize);
    ASSERT(z >= 0 && z < ChunkSize);
    
    return (x * ChunkSize * ChunkSize) + (y * ChunkSize) + z;
}

int GetBlockType(Chunk* aChunk, int x, int y, int z)
{
    return aChunk->myBlocks[GetBlockIndex(x, y, z)];
}

bool SetBlockType(Chunk* aChunk, int x, int y, int z, int aNewBlockType)
{
    int currentType = GetBlockType(aChunk, x, y, z);
    if(currentType == aNewBlockType)
        return false;
    
    aChunk->myBlocks[GetBlockIndex(x, y, z)] = aNewBlockType;
    return true;
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
    
    int stoneLevel = 20;
    int grassLevel = 13;
    int dirtLevel = 7;
    int waterLevel = 5;
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
                
                int terrainHeight = static_cast<int>(value * 30.f);
                terrainHeight = max(terrainHeight, waterLevel);
                
                
                int blockValue = InvalidBlockType;
                if(voxelY <= terrainHeight)
                {
                    if(voxelY > stoneLevel)
                        blockValue = Stone;
                    else if(voxelY > grassLevel)
                        blockValue = Grass;
                    else if(voxelY > dirtLevel)
                        blockValue = Dirt;
                    else
                        blockValue = Water;
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
                    if(GetBlockType(aChunk, x - 1, y, z) == InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x + 1, y, z) == InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y - 1, z) == InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y + 1, z) == InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y, z - 1) == InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(GetBlockType(aChunk, x, y, z + 1) == InvalidBlockType)
                        fullySurrounded = false;
                    
                    if(fullySurrounded)
                        continue;
                }
                
                Vector4f color = {0.f, 0.f, 0.f, 1.f};
                
                switch(blockType)
                {
                    case Grass: color = {0.05f, 0.75f, 0.1f, 1.f}; break;
                    case Dirt: color = {77.f/256.f, 39.f/256.f, 14.f/256.f, 1.f}; break;
                    case Stone: color = {87.f/256.f, 83.f/256.f, 80.f/256.f, 1.f}; break;
                    case Water: color = {0.f/256.f, 25.f/256.f, 225.f/256.f, 1.f}; break;
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

void DoRaycastTraversal(const DE_Ray& aMouseRay, const ChunkSectionAABB& aSection, bool aIgnoreInvalidBlock, ChunkRaycastHit& outHits)
{
    Vector3f minPos = aSection.myChunk->myWorldPosition;
    minPos.x += aSection.myX - 0.5f;
    minPos.y += aSection.myY - 0.5f;
    minPos.z += aSection.myZ - 0.5f;
    
    Vector3f maxPos = minPos;
    maxPos.x += aSection.mySize;
    maxPos.y += aSection.mySize;
    maxPos.z += aSection.mySize;
    
    DE_AABB aabb = AABBFromPoints(minPos, maxPos);
    Vector3f hitPosition;
    float distance = LineVSAABB(aabb, aMouseRay, &hitPosition);
    
    
    if(distance > -1.f)
    {
        if(aSection.mySize > 1)
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
                        
                        DoRaycastTraversal(aMouseRay, section, aIgnoreInvalidBlock, outHits);
                    }
                }
            }
        }
        else
        {
            int blockType = GetBlockType(aSection.myChunk, aSection.myX, aSection.myY, aSection.myZ);
            if(!aIgnoreInvalidBlock || blockType != InvalidBlockType)
            {
                BlockRaycastHit& hit = ArrayAdd(outHits.myBlockHits);
                hit.myX = aSection.myX;
                hit.myY = aSection.myY;
                hit.myZ = aSection.myZ;
                hit.myRaycastDistance = distance;
                hit.myHitPosition = hitPosition;
                
                if(distance < outHits.myClosestHit.myRaycastDistance)
                    outHits.myClosestHit = hit;
            }
        }
    }
}

bool DoRaycast(const DE_Ray& aMouseRay, bool aIgnoreInvalidBlocks, GrowingArray<ChunkRaycastHit>& outHits)
{
    ChunkSectionAABB section;
    section.myX = 0;
    section.myY = 0;
    section.myZ = 0;
    section.mySize = ChunkSize;
    
    for(int i = 0; i < ourGameState.myWorld.myChunksToRender.myCount; ++i)
    {
        Chunk* chunk = ourGameState.myWorld.myChunksToRender[i];
        section.myChunk = chunk;
        
        ChunkRaycastHit chunkHit = {};
        chunkHit.myChunk = chunk;
        chunkHit.myClosestHit.myRaycastDistance = FLT_MAX;
        ArrayAlloc(chunkHit.myBlockHits, 16);

        DoRaycastTraversal(aMouseRay, section, aIgnoreInvalidBlocks, chunkHit);
        
        if(chunkHit.myClosestHit.myRaycastDistance < FLT_MAX)
        {
            ArrayAdd(outHits, chunkHit);
        }
    }
    
    return outHits.myCount > 0;
}

void ModifyBlockUnderMouse(const DE_Ray& aMouseRay, int aNewBlockType)
{
    GrowingArray<ChunkRaycastHit> raycastHits;
    ArrayAlloc(raycastHits, 16);
    
    DoRaycast(aMouseRay, true, raycastHits);
    
    int bestHit = -1;
    float bestDistance = FLT_MAX;
    for(int i = 0; i < raycastHits.myCount; ++i)
    {
        ChunkRaycastHit& chunkHit = raycastHits[i];
        if(chunkHit.myClosestHit.myRaycastDistance < bestDistance)
        {
            bestDistance = chunkHit.myClosestHit.myRaycastDistance;
            bestHit = i;
        }
    }
    
    if(bestHit != -1)
    {
        ChunkRaycastHit& chunkHit = raycastHits[bestHit];
        BlockRaycastHit& hit = chunkHit.myClosestHit;
        if(SetBlockType(chunkHit.myChunk, hit.myX, hit.myY, hit.myZ, aNewBlockType))
            ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, chunkHit.myChunk);
    }
}
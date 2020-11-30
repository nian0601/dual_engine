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

Chunk* FindChunk(Vector3i position)
{
    for(Chunk* chunk : ourGameState.myWorld.myChunks)
    {
        if(chunk->myChunkPosition == position)
            return chunk;
    }
    
    return nullptr;
}

Chunk* CreateEmptyChunk(int x, int y, int z)
{
    Chunk* chunk = new Chunk();
    chunk->myChunkPosition.x = x;
    chunk->myChunkPosition.y = y;
    chunk->myChunkPosition.z = z;
    chunk->myWorldPosition.x = x * ChunkSize;
    chunk->myWorldPosition.y = y * ChunkSize;
    chunk->myWorldPosition.z = z * ChunkSize;
    chunk->myAABB = AABBFromExtents(chunk->myWorldPosition, ChunkExtents);
    chunk->myMeshID = -1;
    chunk->myState = Chunk::ChunkState::UNINITIALIZED;
    
    ArrayAdd(ourGameState.myWorld.myChunks, chunk);
    ArrayAdd(ourGameState.myWorld.myUninitializedChunks, chunk);
    
    return chunk;
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
        int voxelX = aChunk->myChunkPosition.x * ChunkSize + x;
        
        for(int y = 0; y < ChunkSize; ++y)
        {
            int voxelY = aChunk->myChunkPosition.y * ChunkSize + y;
            
            for(int z = 0; z < ChunkSize; ++z)
            {
                int voxelZ = aChunk->myChunkPosition.z * ChunkSize + z;
                
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
        aChunk->myMeshID = OpenGL_CreateMesh();
    
    OpenGL_ClearMesh(aChunk->myMeshID);
    
    for(int x = 0; x < ChunkSize; ++x)
    {       
        for(int y = 0; y < ChunkSize; ++y)
        {
            for(int z = 0; z < ChunkSize; ++z)
            {
                int blockType = GetBlockType(aChunk, x, y, z);
                
                if(blockType == InvalidBlockType)
                    continue;
                
                Vector4f color = {0.f, 0.f, 0.f, 1.f};
                
                switch(blockType)
                {
                    case Grass: color = {0.05f, 0.75f, 0.1f, 1.f}; break;
                    case Dirt: color = {77.f/256.f, 39.f/256.f, 14.f/256.f, 1.f}; break;
                    case Stone: color = {87.f/256.f, 83.f/256.f, 80.f/256.f, 1.f}; break;
                    case Water: color = {0.f/256.f, 25.f/256.f, 225.f/256.f, 1.f}; break;
                }
                
                int meshFlags = 0;
                
                if(x == 0 || GetBlockType(aChunk, x - 1, y, z) == InvalidBlockType)
                    meshFlags |= OpenGL_CubeMeshFlags::LEFT;
                
                if(x == ChunkSize - 1 || GetBlockType(aChunk, x + 1, y, z) == InvalidBlockType)
                    meshFlags |= OpenGL_CubeMeshFlags::RIGHT;
                
                if(y == 0 || GetBlockType(aChunk, x, y - 1, z) == InvalidBlockType)
                    meshFlags |= OpenGL_CubeMeshFlags::BOTTOM;
                
                if(y == ChunkSize - 1 || GetBlockType(aChunk, x, y + 1, z) == InvalidBlockType)
                    meshFlags |= OpenGL_CubeMeshFlags::TOP;
                
                if(z == 0 || GetBlockType(aChunk, x, y, z - 1) == InvalidBlockType)
                    meshFlags |= OpenGL_CubeMeshFlags::FRONT;
                
                if(z == ChunkSize - 1 || GetBlockType(aChunk, x, y, z + 1) == InvalidBlockType)
                    meshFlags |= OpenGL_CubeMeshFlags::BACK;
                
                OpenGL_CreateCubeMesh(aChunk->myMeshID, float(x), float(y), float(z), color.x, color.y, color.z, meshFlags);
            }
        }
    }
    
    OpenGL_FinishMesh(aChunk->myMeshID);
}

void ChunkWorkerUpdateFunction();
void CreateWorld()
{
    World& world = ourGameState.myWorld;
    ArrayAlloc(world.myChunks, ChunkStreamingBorderSize * ChunkStreamingBorderSize * ChunkStreamingBorderSize);
    ArrayAlloc(world.myUninitializedChunks, ChunkStreamingBorderSize);
    ArrayAlloc(world.myActiveChunks, ChunkStreamingBorderSize);
    ArrayAlloc(world.myFrozenChunks, ChunkStreamingBorderSize);
    
    ArrayAlloc(world.myChunksToBuildMesh, ChunkStreamingBorderSize);
    
    ArrayAlloc(world.myChunkWorkerData.myWaitingList, ChunkStreamingBorderSize);
    ArrayAlloc(world.myChunkWorkerData.myActiveList, ChunkStreamingBorderSize);
    ArrayAlloc(world.myChunkWorkerData.myFinishedList, ChunkStreamingBorderSize);
    
    world.myStreamingCenterChunk.x = -1;
    world.myStreamingCenterChunk.y = -1;
    world.myStreamingCenterChunk.z = -1;
    
    world.myChunkWorkerData.myThread = new std::thread(ChunkWorkerUpdateFunction);
}

void DestroyWorld()
{
    World& world = ourGameState.myWorld;
    world.myChunkWorkerData.myShutDownWorker = true;
    world.myChunkWorkerData.myThread->join();
}

void RenderChunk(Chunk* aChunk)
{
    if(aChunk->myMeshID != -1)
        QueueMesh(aChunk->myMeshID, aChunk->myWorldPosition);
}

void RenderWorld()
{
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myActiveChunks;
    for(Chunk* chunk : chunks)
        RenderChunk(chunk);
}

void UpdateStreamingArea(const Vector3f aStreamingPosition)
{
    Vector3i centerChunk;
    
    centerChunk.x = int(aStreamingPosition.x / ChunkSize);
    centerChunk.y = 0;
    centerChunk.z = int(aStreamingPosition.z / ChunkSize);
    
    if(centerChunk == ourGameState.myWorld.myStreamingCenterChunk)
        return;
    
    ourGameState.myWorld.myStreamingCenterChunk = centerChunk;

    Vector3i minPos = centerChunk - ChunkStreamingBorderSize;
    minPos.x = Max(minPos.x, 0);
    minPos.y = 0;
    minPos.z = Max(minPos.z, 0);
    Vector3i maxPos = centerChunk + ChunkStreamingBorderSize;
    maxPos.y = WorldHeight;
    DE_AABBi streamingArea = AABBiFromPoints(minPos, maxPos);
    
    // Go through all existing chunks.
    // Any ACTIVE chunks that now are OUTSIDE the streaming area needs to become FROZEN
    // Any FROZEN chunks that now are INSIDE the streaming area needs to become ACTIVE
    //
    // We do not create any new chunks here, we'll handle that separately
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myChunks;
    for(Chunk* chunk : chunks)
    {
        bool isInsideStreamingArea = AABBvsPoint(streamingArea, chunk->myChunkPosition);
        
        if(isInsideStreamingArea && chunk->myState == Chunk::ChunkState::FROZEN)
        {
            chunk->myState = Chunk::ChunkState::ACTIVE;
            
            ArrayRemoveCyclic(ourGameState.myWorld.myFrozenChunks, chunk);
            ArrayAdd(ourGameState.myWorld.myActiveChunks, chunk);
            
            // When we're actually deleting the mesh when freezing chunks when we'll
            // need to also rebuild the mesh when unfreezing them
            //ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, chunk);
        }
        else if(!isInsideStreamingArea && chunk->myState == Chunk::ChunkState::ACTIVE)
        {
            chunk->myState = Chunk::ChunkState::FROZEN;
            
            ArrayRemoveCyclic(ourGameState.myWorld.myActiveChunks, chunk);
            ArrayAdd(ourGameState.myWorld.myFrozenChunks, chunk);
            
            // We should be fully destroying the mesh here as well to avoid keeping
            // all that meshdata in memory. Should update the OpenGL-interface to support that.
            // Will need a smarter system for MeshID-handling though.
            //
            //ArrayAdd(ourGameState.myWorld.myChunksToDestroyMesh, chunk);
        }
    }
    
    // Now we go through all the chunks inside the streamingarea and
    // create new chunks where we need to
    for(int x = minPos.x; x <= maxPos.x; ++x)
    {
        for(int y = minPos.y; y <= maxPos.y; ++y)
        {
            for(int z = minPos.z; z <= maxPos.z; ++z)
            {
                Chunk* chunk = FindChunk({x, y, z});
                if(!chunk)
                    CreateEmptyChunk(x, y, z);
            }
        }
    }
}

void ChunkWorkerFillChunk(Chunk* aChunk, FastNoise& someNoise)
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
        int voxelX = aChunk->myChunkPosition.x * ChunkSize + x;
        
        for(int y = 0; y < ChunkSize; ++y)
        {
            int voxelY = aChunk->myChunkPosition.y * ChunkSize + y;
            
            for(int z = 0; z < ChunkSize; ++z)
            {
                int voxelZ = aChunk->myChunkPosition.z * ChunkSize + z;
                
                float value = someNoise.GetSimplex(voxelX, voxelZ);
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

void ChunkWorkerUpdateFunction()
{
    ChunkWorkerData& workerData = ourGameState.myWorld.myChunkWorkerData;
    while(!workerData.myShutDownWorker)
    {
        {
            ReadWriteLock lock(workerData.myMutex);
            
            // First we move all the chunks we finished the last tick to the finished list
            // And clear the ActiveList
            for(Chunk* finishedChunk : workerData.myActiveList)
                ArrayAdd(workerData.myFinishedList, finishedChunk);
            
            ArrayClear(workerData.myActiveList);
            
            // Then we move all the new Waiting chunks from the WaitingList to the ActiveList
            // and clear the WaitingList.
            
            for(Chunk* waitingChunk : workerData.myWaitingList)
                ArrayAdd(workerData.myActiveList, waitingChunk);
            
            ArrayClear(workerData.myWaitingList);
            
            // Now we dont need to keep the mutex locked anymore since none else touches the ActiveList
            // The main-thread is "free" to mess around with the Waiting and Finished list, as long
            // as they lock the mutex so that we dont collide when we're finished with the current AcitveList.
        }
        
        // Process all the 'ActiveList' chunks
        for(Chunk* chunk : workerData.myActiveList)
        {
            ChunkWorkerFillChunk(chunk, workerData.myNoise);
        }
    }
}

void SyncronizeChunkWorker()
{
    ChunkWorkerData& workerData = ourGameState.myWorld.myChunkWorkerData;
    ReadWriteLock lock(workerData.myMutex);
    
    // Send chunks that needs to be built to the waitinglist
    for(Chunk* chunk : ourGameState.myWorld.myUninitializedChunks)
    {
        ArrayAdd(workerData.myWaitingList, chunk);
        chunk->myState = Chunk::ChunkState::ON_WORKER_THREAD;
    }
    ArrayClear(ourGameState.myWorld.myUninitializedChunks);
    
    // Grab finished chunks from the worker and put them into the 'ChunksToBuildMesh'-list.
    for(Chunk* chunk : workerData.myFinishedList)
    {
        ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, chunk);
        ArrayAdd(ourGameState.myWorld.myActiveChunks, chunk);
        
        chunk->myState = Chunk::ChunkState::ACTIVE;
    }
    ArrayClear(workerData.myFinishedList);
}

void BuildInitialChunks(const Vector3f& aPosition)
{
    Vector3i centerChunk;
    
    centerChunk.x = int(aPosition.x / ChunkSize);
    centerChunk.y = 0;
    centerChunk.z = int(aPosition.z / ChunkSize);
    
    Vector3i minPos = centerChunk - 3;
    minPos.x = Max(minPos.x, 0);
    minPos.y = 0;
    minPos.z = Max(minPos.z, 0);
    Vector3i maxPos = centerChunk + 3;
    maxPos.y = WorldHeight;
    
    for(int x = minPos.x; x <= maxPos.x; ++x)
    {
        for(int y = minPos.y; y <= maxPos.y; ++y)
        {
            for(int z = minPos.z; z <= maxPos.z; ++z)
            {
                Chunk* chunk = FindChunk({x, y, z});
                if(!chunk)
                    CreateEmptyChunk(x, y, z);
            }
        }
    }
    
    for(Chunk* chunk : ourGameState.myWorld.myUninitializedChunks)
    {
        FillChunk(chunk);
        BuildChunkMesh(chunk);
        
        ArrayAdd(ourGameState.myWorld.myActiveChunks, chunk);
        chunk->myState = Chunk::ChunkState::ACTIVE;
    }
    ArrayClear(ourGameState.myWorld.myUninitializedChunks);
}

void InitChunks()
{
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myUninitializedChunks;
    int chunksProcessed = 0;
    while(chunks.myCount > 0 && chunksProcessed < MaxChunksToCreatePerUpdate)
    {
        Chunk* chunk = chunks[chunks.myCount - 1];
        ArrayRemoveCyclic(chunks, chunks.myCount - 1);
        
        FillChunk(chunk);
        
        ArrayAdd(ourGameState.myWorld.myChunksToBuildMesh, chunk);
        ArrayAdd(ourGameState.myWorld.myActiveChunks, chunk);
     
        chunk->myState = Chunk::ChunkState::ACTIVE;
        ++chunksProcessed;
    }
}

void BuildChunkMeshes()
{
    GrowingArray<Chunk*>& chunks = ourGameState.myWorld.myChunksToBuildMesh;
    int chunksProcessed = 0;
    while(chunks.myCount > 0 && chunksProcessed < MaxChunksToCreatePerUpdate)
    {
        Chunk* chunk = chunks[chunks.myCount - 1];
        ArrayRemoveCyclic(chunks, chunks.myCount - 1);
        
        BuildChunkMesh(chunk);
        
        ++chunksProcessed;
    }
}

void UpdateWorld()
{   
    SyncronizeChunkWorker();
    //InitChunks();
    BuildChunkMeshes();
    
    RenderWorld();
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
    for(Chunk* chunk : ourGameState.myWorld.myActiveChunks)
    {
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
    
    for(Chunk* chunk : ourGameState.myWorld.myActiveChunks)
    {
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

BlockRaycastHit GetClosestBlockHit(const GrowingArray<ChunkRaycastHit>& someChunkHits)
{
    float bestDistance = FLT_MAX;
    int bestHit = -1;
    
    for(int i = 0; i < someChunkHits.myCount; ++i)
    {
        const ChunkRaycastHit& chunkHit = someChunkHits[i];
        if(chunkHit.myClosestHit.myRaycastDistance < bestDistance)
        {
            bestDistance = chunkHit.myClosestHit.myRaycastDistance;
            bestHit = i;
        }
    }
    
    assert(bestHit != -1);
    return someChunkHits[bestHit].myClosestHit;
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
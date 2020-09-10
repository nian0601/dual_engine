void CreateCube(int aMeshID, float x, float y, float z, float r, float g, float b)
{
    // Create our Vertices
    float width = 0.5f;
    float height = 0.5f;
    float depth = 0.5f;
    
    Vector4f p0 = {x - width, y + height, z - depth, 1.f};
    Vector4f p1 = {x + width, y + height, z - depth, 1.f};
    Vector4f p2 = {x + width, y + height, z + depth, 1.f};
    Vector4f p3 = {x - width, y + height, z + depth, 1.f};
    
    Vector4f p4 = {x - width, y - height, z - depth, 1.f};
    Vector4f p5 = {x + width, y - height, z - depth, 1.f};
    Vector4f p6 = {x + width, y - height, z + depth, 1.f};
    Vector4f p7 = {x - width, y - height, z + depth, 1.f};
    
    Vector4f normal;
    Vector4f color = {r, g, b, 1.f};
    
    //0 - 3 (Top)
    normal = {0.f, 1.f, 0.f, 0.f};
    int v0 = gfx_AddVertexToMesh(aMeshID, p0, normal, color);
    int v1 = gfx_AddVertexToMesh(aMeshID, p1, normal, color);
    int v2 = gfx_AddVertexToMesh(aMeshID, p2, normal, color);
    int v3 = gfx_AddVertexToMesh(aMeshID, p3, normal, color);
    
    gfx_AddTriangleToMesh(aMeshID, v3, v1, v0);
    gfx_AddTriangleToMesh(aMeshID, v2, v1, v3);
    
	//4 - 7 (Bottom)
    normal = {0.f, -1.f, 0.f, 0.f};
    int v4 = gfx_AddVertexToMesh(aMeshID, p4, normal, color);
    int v5 = gfx_AddVertexToMesh(aMeshID, p5, normal, color);
    int v6 = gfx_AddVertexToMesh(aMeshID, p6, normal, color);
    int v7 = gfx_AddVertexToMesh(aMeshID, p7, normal, color);
    
    gfx_AddTriangleToMesh(aMeshID, v6, v4, v5);
    gfx_AddTriangleToMesh(aMeshID, v7, v4, v6);
    
	//8 - 11 (Left)
    normal = {-1.f, 0.f, 0.f, 0.f};
    int v8 = gfx_AddVertexToMesh(aMeshID, p7, normal, color);
    int v9 = gfx_AddVertexToMesh(aMeshID, p4, normal, color);
    int v10 = gfx_AddVertexToMesh(aMeshID, p0, normal, color);
    int v11 = gfx_AddVertexToMesh(aMeshID, p3, normal, color);
    
    gfx_AddTriangleToMesh(aMeshID, v11, v9, v8);
    gfx_AddTriangleToMesh(aMeshID, v10, v9, v11);
    
	//12 - 15 (Right)
    normal = {1.f, 0.f, 0.f, 0.f};
    int v12 = gfx_AddVertexToMesh(aMeshID, p6, normal, color);
    int v13 = gfx_AddVertexToMesh(aMeshID, p5, normal, color);
    int v14 = gfx_AddVertexToMesh(aMeshID, p1, normal, color);
    int v15 = gfx_AddVertexToMesh(aMeshID, p2, normal, color);
    
    gfx_AddTriangleToMesh(aMeshID, v14, v12, v13);
    gfx_AddTriangleToMesh(aMeshID, v15, v12, v14);
    
	//16 - 19 (Front)
    normal = {0.f, 0.f, 1.f, 0.f};
    int v16 = gfx_AddVertexToMesh(aMeshID, p4, normal, color);
    int v17 = gfx_AddVertexToMesh(aMeshID, p5, normal, color);
    int v18 = gfx_AddVertexToMesh(aMeshID, p1, normal, color);
    int v19 = gfx_AddVertexToMesh(aMeshID, p0, normal, color);
    
    gfx_AddTriangleToMesh(aMeshID, v19, v17, v16);
    gfx_AddTriangleToMesh(aMeshID, v18, v17, v19);
    
	//20 - 23 (Back)
    normal = {0.f, 0.f, -1.f, 0.f};
    int v20 = gfx_AddVertexToMesh(aMeshID, p7, normal, color);
    int v21 = gfx_AddVertexToMesh(aMeshID, p6, normal, color);
    int v22 = gfx_AddVertexToMesh(aMeshID, p2, normal, color);
    int v23 = gfx_AddVertexToMesh(aMeshID, p4, normal, color);
    
    gfx_AddTriangleToMesh(aMeshID, v22, v20, v21);
    gfx_AddTriangleToMesh(aMeshID, v23, v20, v22);
}

void FillChunk(Chunk* aChunk)
{
    ArrayAlloc(aChunk->myBlocks, ChunkSize * ChunkSize * ChunkSize);
    
    for(int x = 0; x < ChunkSize; ++x)
    {
        for(int y = 0; y < ChunkSize; ++y)
        {
            for(int z = 0; z < ChunkSize; ++z)
            {
                Block& block = ArrayAdd(aChunk->myBlocks);
                block.myBlockType = static_cast<BlockType>(rand() % 3);
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
                int index = (x * (ChunkSize-1)) + (y * ChunkSize) + z;
                
                Vector4f color;
                switch(aChunk->myBlocks[index].myBlockType)
                {
                    case Type1: color = {0.54f, 0.57f, 0.54f, 1.f}; break;
                    case Type2: color = {0.54f, 0.54f, 0.51f, 1.f}; break;
                    case Type3: color = {0.55f, 0.51f, 0.52f, 1.f}; break;
                }
                
                CreateCube(aChunk->myMeshID, float(x), float(y), float(z), color.x, color.y, color.z);
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
    
    for(int x = 0; x < WorldSize; x++)
    {
        for(int y = 0; y < WorldSize; ++y)
        {
            for(int z = 0; z < WorldSize; ++z)
            {
                Vector3f chunkPosition;
                chunkPosition.x = float(x);
                chunkPosition.y = float(y);
                chunkPosition.z = float(z);
                
                Chunk* chunk = new Chunk();
                chunk->myChunkPosition = chunkPosition;   
                chunk->myWorldPosition = chunkPosition * ChunkSize;
                
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
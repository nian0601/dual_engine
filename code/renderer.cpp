struct QuadData
{
    unsigned int myTextureID;
    Vector2f myPosition;
    Vector2f mySize;
    Vector4f myColor;
    bool myIsText;
};

struct CubeData
{
    Vector3f myPosition;
    Vector3f mySize;
    Vector4f myColor;
};

struct MeshData
{
    int myMeshID;
    Vector3f myPosition;
};

struct RenderContext
{
    Matrix myDummyTransform;
    GrowingArray<QuadData> myQuads;
    GrowingArray<CubeData> myCubes;
    GrowingArray<MeshData> myMeshes;
};

static RenderContext ourRenderContext;

void SetupRenderer()
{
    ourRenderContext = {};
    ourRenderContext.myDummyTransform = IdentityMatrix();
    
    ArrayAlloc(ourRenderContext.myQuads, 16);
    ArrayAlloc(ourRenderContext.myCubes, 16);
    ArrayAlloc(ourRenderContext.myMeshes, 16);
}

void QueueCube(const Vector3f& aPosition, const Vector3f& aSize, const Vector4f& aColor)
{
    ArrayAdd(ourRenderContext.myCubes, {aPosition, aSize, aColor});
}

void QueueCube(const Vector3f& aPosition, const Vector4f& aColor)
{
    ArrayAdd(ourRenderContext.myCubes, {aPosition, {1.f, 1.f, 1.f}, aColor});
}

void QueueQuad(unsigned int aTextureID, const Vector2f& aPosition, const Vector2f& aSize)
{
    ArrayAdd(ourRenderContext.myQuads, {aTextureID, aPosition, aSize, {1.f, 1.f, 1.f, 1.f}, false});
}

void QueueMesh(int aMeshID, const Vector3f& aPosition)
{
    ArrayAdd(ourRenderContext.myMeshes, {aMeshID, aPosition});
}

void QueueText(const Vector2f& aPosition, const char* aString)
{
    AssetInfo assetInfo;
    
    Vector2f currentPoint = aPosition;
    Vector2f tempPoint = currentPoint;
    int length = strlen(aString);
    for(int i = 0; i < length; ++i)
    {
        assetInfo = GetCharBitmap(aString[i]);
        tempPoint.y -= assetInfo.myBoundingBox.w;
        
        ArrayAdd(ourRenderContext.myQuads, {assetInfo.myTextureID, tempPoint, assetInfo.mySize, {0.f, 0.f, 0.f, 1.f}, true});
        
        tempPoint.y += 4.f;
        tempPoint.x -= 4.f;
        
        ArrayAdd(ourRenderContext.myQuads, {assetInfo.myTextureID, tempPoint, assetInfo.mySize, {1.f, 1.f, 1.f, 1.f}, true});
        
        currentPoint.x += assetInfo.myAdvance;
        tempPoint = currentPoint;
    }
}

void PushRendererData()
{
    gfx_Begin3D();
    
    for(int i = 0; i < ourRenderContext.myCubes.myCount; ++i)
    {
        ourRenderContext.myDummyTransform = IdentityMatrix();
        
        const CubeData& data = ourRenderContext.myCubes[i];
        Translate(ourRenderContext.myDummyTransform, data.myPosition);
        gfx_DrawCube(ourRenderContext.myDummyTransform, data.myColor);
    }

    ArrayClear(ourRenderContext.myCubes);
    
    for(int i = 0; i < ourRenderContext.myMeshes.myCount; ++i)
    {
        ourRenderContext.myDummyTransform = IdentityMatrix();
        
        const MeshData& data = ourRenderContext.myMeshes[i];
        Translate(ourRenderContext.myDummyTransform, data.myPosition);
        gfx_DrawMesh(data.myMeshID, ourRenderContext.myDummyTransform);
    }
    
    ArrayClear(ourRenderContext.myMeshes);
    
    for(int i = 0; i < ourRenderContext.myQuads.myCount; ++i)
    {
        const QuadData& data = ourRenderContext.myQuads[i];
        if(data.myIsText)
            gfx_BeginText();
        else
            gfx_Begin2D();
        
        gfx_DrawQuad(
            data.myTextureID,
            data.myPosition.x,
            data.myPosition.y,
            data.mySize.x,
            data.mySize.y,
            data.myColor);
    }
    
    ArrayClear(ourRenderContext.myQuads);
}


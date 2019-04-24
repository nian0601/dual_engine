struct QuadData
{
    int myTextureID;
    Vector2f myPosition;
    Vector2f mySize;
};

struct CubeData
{
    Vector3f myPosition;
    Vector3f mySize;
    Vector4f myColor;
};

struct RenderContext
{
    Matrix myDummyTransform;
    GrowingArray<QuadData> myQuads;
    GrowingArray<CubeData> myCubes;
};

static RenderContext ourRenderContext;

void SetupRenderer()
{
    ourRenderContext = {};
    ourRenderContext.myDummyTransform = IdentityMatrix();
    
    ArrayAlloc(ourRenderContext.myQuads, 12);
    ArrayAlloc(ourRenderContext.myCubes, 12);
}

void QueueCube(const Vector3f& aPosition, const Vector3f& aSize, const Vector4f& aColor)
{
    ArrayAdd(ourRenderContext.myCubes, {aPosition, aSize, aColor});
}

void QueueCube(const Vector3f& aPosition, const Vector4f& aColor)
{
    ArrayAdd(ourRenderContext.myCubes, {aPosition, {1.f, 1.f, 1.f}, aColor});
}

void QueueQuad(int aTextureID, const Vector2f& aPosition, const Vector2f& aSize)
{
    ArrayAdd(ourRenderContext.myQuads, {aTextureID, aPosition, aSize});
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
        QueueQuad(assetInfo.myTextureID, tempPoint, assetInfo.mySize);
        
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
    
    gfx_Begin2D();
    
    for(int i = 0; i < ourRenderContext.myQuads.myCount; ++i)
    {
        const QuadData& data = ourRenderContext.myQuads[i];
        gfx_DrawQuad(
            data.myTextureID,
            data.myPosition.x,
            data.myPosition.y,
            data.mySize.x,
            data.mySize.y);
    }
    
    ArrayClear(ourRenderContext.myQuads);
}


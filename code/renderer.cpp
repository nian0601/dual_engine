struct Camera
{
    Matrix myProjection;
    Matrix myView;
    Matrix myInvertedView;
    Vector2f myWindowSize;
    Vector2f myScreenSize;
};


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
    
    int myCubeMeshID;
    int myQuadMeshID;
    
    unsigned int myMeshShader;
    unsigned int myQuadShader;
    unsigned int myTextShader;
};

static RenderContext ourRenderContext;

void SetupRenderer()
{
    ourRenderContext = {};
    ourRenderContext.myDummyTransform = IdentityMatrix();
    
    ArrayAlloc(ourRenderContext.myQuads, 16);
    ArrayAlloc(ourRenderContext.myCubes, 16);
    ArrayAlloc(ourRenderContext.myMeshes, 16);
    
    ourRenderContext.myMeshShader = OpenGL_CreateShader("mesh.vx", "mesh.px");
    ourRenderContext.myQuadShader = OpenGL_CreateShader("quad.vx", "quad.px");
    ourRenderContext.myTextShader = OpenGL_CreateShader("text.vx", "text.px");
    
    int meshID = OpenGL_CreateMesh();
    OpenGL_CreateCubeMesh(meshID, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, OpenGL_CubeMeshFlags::ALL);
    OpenGL_FinishMesh(meshID);
    ourRenderContext.myCubeMeshID = meshID;
    
    ourRenderContext.myQuadMeshID = OpenGL_CreateQuadMesh();
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

void PushMeshData(Camera& aCamera)
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    
    OpenGL_ActivateShader(ourRenderContext.myMeshShader);
    OpenGL_SetShaderUniform("Projection", aCamera.myProjection);
    OpenGL_SetShaderUniform("View", aCamera.myInvertedView);
    
    for(int i = 0; i < ourRenderContext.myCubes.myCount; ++i)
    {
        ourRenderContext.myDummyTransform = IdentityMatrix();
        
        const CubeData& data = ourRenderContext.myCubes[i];
        Translate(ourRenderContext.myDummyTransform, data.myPosition);
        
        OpenGL_SetShaderUniform("World", ourRenderContext.myDummyTransform);
        OpenGL_SetShaderUniform("ColorAndMetalnessIn", data.myColor);
        OpenGL_DrawMesh(ourRenderContext.myCubeMeshID);
    }
    
    ArrayClear(ourRenderContext.myCubes);
    
    Vector4f color = {1.f, 1.f, 1.f, 1.f};
    for(int i = 0; i < ourRenderContext.myMeshes.myCount; ++i)
    {
        ourRenderContext.myDummyTransform = IdentityMatrix();
        
        const MeshData& data = ourRenderContext.myMeshes[i];
        Translate(ourRenderContext.myDummyTransform, data.myPosition);
        
        OpenGL_SetShaderUniform("World", ourRenderContext.myDummyTransform);
        OpenGL_SetShaderUniform("ColorAndMetalnessIn", color);
        OpenGL_DrawMesh(data.myMeshID);
    }
    
    ArrayClear(ourRenderContext.myMeshes);
}

void PushQuadData(Camera& aCamera)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    
    for(int i = 0; i < ourRenderContext.myQuads.myCount; ++i)
    {
        
        const QuadData& data = ourRenderContext.myQuads[i];
        if(data.myIsText)
        {
            OpenGL_ActivateShader(ourRenderContext.myTextShader);
            OpenGL_SetShaderUniform("AlbedoTexture", 0);
        }
        else
        {   
            OpenGL_ActivateShader(ourRenderContext.myQuadShader);
            OpenGL_SetShaderUniform("AlbedoTexture", 0);
        }
        
        const Vector2f& screenSize = aCamera.myScreenSize;
        
        Vector2f normalizedPosition = data.myPosition / screenSize;
        Vector2f normalizedSize = data.mySize / screenSize;
        
        glBindTexture(GL_TEXTURE_2D, data.myTextureID);
        OpenGL_SetShaderUniform("Position", normalizedPosition);
        OpenGL_SetShaderUniform("Size", normalizedSize);
        OpenGL_SetShaderUniform("Color", data.myColor);
        
        OpenGL_DrawMesh(ourRenderContext.myQuadMeshID);
    }
    
    ArrayClear(ourRenderContext.myQuads);
}

void PushRendererData(Camera& aCamera)
{
    PushMeshData(aCamera);
    PushQuadData(aCamera);
}


struct OpenGL_cubeVertex
{
    Vector4f myPosition;
    Vector4f myNormal;
    Vector4f myColor;
};

struct OpenGL_renderobject
{  
    unsigned int myVertexArrayObject;
    unsigned int myVertexBufferObject;
    unsigned int myElementBufferObject;
    
    GrowingArray<OpenGL_cubeVertex> myVertices;
    GrowingArray<int> myIndices;
};

struct OpenGL_context
{
    GrowingArray<OpenGL_renderobject> myMeshes;
    unsigned int myActiveShader;
};

static OpenGL_context ourOpenGL_Context;

void OpenGL_VerifyShader(unsigned int aShaderID)
{
    int success = 0;
    glGetShaderiv(aShaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(aShaderID, 512, NULL, infoLog);
        ASSERT(false);
    }
}

void OpenGL_VerifyShaderProgram(unsigned int aShaderProgramID)
{
    int success = 0;
    glGetProgramiv(aShaderProgramID, GL_LINK_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(aShaderProgramID, 512, NULL, infoLog);
        ASSERT(false);
    }
}

unsigned int OpenGL_CreateShader(const char* aVertexName, const char* aPixelName)
{
    char fullPath[100];
    
    // Compile our VertexShader
    snprintf(fullPath, 100, "data/shaders/%s", aVertexName);
    DE_File vertexFile = DE_ReadEntireFile(fullPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexFile.myContents, NULL);
    glCompileShader(vertexShader);
    OpenGL_VerifyShader(vertexShader);
    DE_FreeFile(vertexFile);
    
    // Compile our FragmentShader
    snprintf(fullPath, 100, "data/shaders/%s", aPixelName);
    DE_File pixelFile = DE_ReadEntireFile(fullPath);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &pixelFile.myContents, NULL);
    glCompileShader(fragmentShader);
    OpenGL_VerifyShader(fragmentShader);
    DE_FreeFile(pixelFile);
    
    // Link our Vertex and Fragment Shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    OpenGL_VerifyShaderProgram(shaderProgram);

    // After we have linked the shaders, we dont need the objects anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

// OpenGL_interface-implementation
void OpenGL_Init()
{
    ArrayAlloc(ourOpenGL_Context.myMeshes, 16);
    ourOpenGL_Context.myActiveShader = -1;
}

void OpenGL_Viewport(int aX, int aY, int aWidth, int aHeight)
{
    glViewport(aX, aY, aWidth, aHeight);
}

void OpenGL_ClearColor(float aR, float aG, float aB)
{
    glClearColor(aR, aG, aB, 1.f);
}

void OpenGL_Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGL_FinishFrame()
{
    glFinish();
}

unsigned int OpenGL_CreateTexture(int aWidth, int aHeight, gfxTextureFormat aTextureFormat, void* someTextureData)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    int format = GL_RGBA;
    if(aTextureFormat == SINGLE_CHANNEL)
        format = GL_RED;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, aWidth, aHeight, 0, format, GL_UNSIGNED_BYTE, someTextureData);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    return texture;
}

void OpenGL_DrawMesh(int aMeshID)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    
    glBindVertexArray(mesh.myVertexArrayObject);
    
    glDrawElements(GL_TRIANGLES, mesh.myIndices.myCount, GL_UNSIGNED_INT, 0);
    
    glBindVertexArray(0);
}

int OpenGL_CreateMesh()
{
    OpenGL_renderobject& mesh = ArrayAdd(ourOpenGL_Context.myMeshes);
    ArrayAlloc(mesh.myVertices, 24);
    ArrayAlloc(mesh.myIndices, 32);
    
    // Create and Bind VertexArrayObject
    // This is the OpenGL-thingy that collects VertexBuffer, IndexBuffer and VertexAttributes under one ID
    // Then we only need to bind this object when rendering
    glGenVertexArrays(1, &mesh.myVertexArrayObject);
    glBindVertexArray(mesh.myVertexArrayObject);
    
    
    // Create VertexBuffer + IndexBuffer
    glGenBuffers(1, &mesh.myVertexBufferObject);
    glGenBuffers(1, &mesh.myElementBufferObject);
    
    // Bind Vertex and Index Buffers to associate it with the VertexArrayObject
    glBindBuffer(GL_ARRAY_BUFFER, mesh.myVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.myElementBufferObject);
 
    
    // Setup the VertexAttributes (InputLayout)
    // aPos (vec4)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // aNormal (vec4)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // aColor (vec4)
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    return ourOpenGL_Context.myMeshes.myCount - 1;
}

int OpenGL_CreateQuadMesh()
{
    OpenGL_renderobject& quad = ArrayAdd(ourOpenGL_Context.myMeshes);
    
    glGenVertexArrays(1, &quad.myVertexArrayObject);
    glBindVertexArray(quad.myVertexArrayObject);
    
    // Create our Vertices
    float size = 0.5f;
    float vertices[] = {
        // position         // colors            //UVs
        size,  size, 0.f,   1.f,  0.f,  0.f,    1.f, 0.f,  // topright
        size, -size, 0.f,   0.f,  1.f,  0.f,    1.f, 1.f,   // bottomright
        -size, -size, 0.f,   0.f,  0.f,  1.f,    0.f, 1.f,   // bottomleft
        -size,  size, 0.f,   0.2f, 0.2f, 0.2f,   0.f, 0.f     // topleft
    };
    
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    ArrayAlloc(quad.myIndices, 6);
    ArrayAdd(quad.myIndices, 0);
    ArrayAdd(quad.myIndices, 1);
    ArrayAdd(quad.myIndices, 3);
    
    ArrayAdd(quad.myIndices, 1);
    ArrayAdd(quad.myIndices, 2);
    ArrayAdd(quad.myIndices, 3);
    
    // Create a VertexBuffer
    glGenBuffers(1, &quad.myVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, quad.myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Create a IndexBuffer
    glGenBuffers(1, &quad.myElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad.myIndices[0]) * quad.myIndices.myCount, quad.myIndices.myData, GL_STATIC_DRAW);
    
    // Setup VertexAttributes (input-layout)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    return ourOpenGL_Context.myMeshes.myCount - 1;
}

void OpenGL_ClearMesh(int aMeshID)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    ArrayClear(mesh.myVertices);
    ArrayClear(mesh.myIndices);
}

int OpenGL_AddVertexToMesh(int aMeshID, const Vector4f& aPosition, const Vector4f& aNormal, const Vector4f& aColor)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    OpenGL_cubeVertex& vertex = ArrayAdd(mesh.myVertices);
    
    vertex.myPosition = aPosition;
    vertex.myNormal = aNormal;
    vertex.myColor = aColor;
    
    return mesh.myVertices.myCount - 1;
}

void OpenGL_AddTriangleToMesh(int aMeshID, int aFirstVertexIndex, int aSecondVertexIndex, int aThirdVertexIndex)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    ArrayAdd(mesh.myIndices, aFirstVertexIndex);
    ArrayAdd(mesh.myIndices, aSecondVertexIndex);
    ArrayAdd(mesh.myIndices, aThirdVertexIndex);
}

void OpenGL_FinishMesh(int aMeshID)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    
    //Bind VertexBuffer + fill it with data
    glBindBuffer(GL_ARRAY_BUFFER, mesh.myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh.myVertices[0]) * mesh.myVertices.myCount, mesh.myVertices.myData, GL_STATIC_DRAW);

    //Bind IndexBuffer + fill it with data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mesh.myIndices[0]) * mesh.myIndices.myCount, mesh.myIndices.myData, GL_STATIC_DRAW);
}

void OpenGL_CreateCubeMesh(int aMeshID, float x, float y, float z, float r, float g, float b, int someFlags)
{
    // Create our Vertices
    float width = 0.5f;
    float height = 0.5f;
    float depth = 0.5f;
    
    Vector4f normal;
    Vector4f color = {r, g, b, 1.f};
    
    //0 - 3 (Top)
    if((someFlags & OpenGL_CubeMeshFlags::TOP) > 0)
    {
        normal = {0.f, 1.f, 0.f, 0.f};
        int v0 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y + height, z - depth, 1.f }, normal, color);
        int v1 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y + height, z - depth, 1.f }, normal, color);
        int v2 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y + height, z + depth, 1.f }, normal, color);
        int v3 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y + height, z + depth, 1.f }, normal, color);
        
        OpenGL_AddTriangleToMesh(aMeshID, v3, v1, v0);
        OpenGL_AddTriangleToMesh(aMeshID, v2, v1, v3);
    }
    
	//4 - 7 (Bottom)
    if((someFlags & OpenGL_CubeMeshFlags::BOTTOM) > 0)
    {
        normal = {0.f, -1.f, 0.f, 0.f};
        int v4 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y - height, z - depth, 1.f }, normal, color);
        int v5 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y - height, z - depth, 1.f }, normal, color);
        int v6 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y - height, z + depth, 1.f }, normal, color);
        int v7 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y - height, z + depth, 1.f }, normal, color);
        
        OpenGL_AddTriangleToMesh(aMeshID, v6, v4, v5);
        OpenGL_AddTriangleToMesh(aMeshID, v7, v4, v6);
    }
    
    //8 - 11 (Left)
    if((someFlags & OpenGL_CubeMeshFlags::LEFT) > 0)
    {
        normal = {-1.f, 0.f, 0.f, 0.f};
        int v8 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y - height, z + depth, 1.f }, normal, color);
        int v9 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y - height, z - depth, 1.f }, normal, color);
        int v10 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y + height, z - depth, 1.f }, normal, color);
        int v11 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y + height, z + depth, 1.f }, normal, color);
        
        OpenGL_AddTriangleToMesh(aMeshID, v11, v9, v8);
        OpenGL_AddTriangleToMesh(aMeshID, v10, v9, v11);
    }
        
	//12 - 15 (Right)
    if((someFlags & OpenGL_CubeMeshFlags::RIGHT) > 0)
    {
        normal = {1.f, 0.f, 0.f, 0.f};
        int v12 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y - height, z + depth, 1.f }, normal, color);
        int v13 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y - height, z - depth, 1.f }, normal, color);
        int v14 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y + height, z - depth, 1.f }, normal, color);
        int v15 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y + height, z + depth, 1.f }, normal, color);
        
        OpenGL_AddTriangleToMesh(aMeshID, v14, v12, v13);
        OpenGL_AddTriangleToMesh(aMeshID, v15, v12, v14);
    }
    
	//16 - 19 (Front)
    if((someFlags & OpenGL_CubeMeshFlags::FRONT) > 0)
    {
        normal = {0.f, 0.f, -1.f, 0.f};
        int v16 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y - height, z - depth, 1.f }, normal, color);
        int v17 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y - height, z - depth, 1.f }, normal, color);
        int v18 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y + height, z - depth, 1.f }, normal, color);
        int v19 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y + height, z - depth, 1.f }, normal, color);
        
        OpenGL_AddTriangleToMesh(aMeshID, v19, v17, v16);
        OpenGL_AddTriangleToMesh(aMeshID, v18, v17, v19);
    }
    
	//20 - 23 (Back)
    if((someFlags & OpenGL_CubeMeshFlags::BACK) > 0)
    {
        normal = {0.f, 0.f, 1.f, 0.f};
        int v20 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y - height, z + depth, 1.f }, normal, color);
        int v21 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y - height, z + depth, 1.f }, normal, color);
        int v22 = OpenGL_AddVertexToMesh(aMeshID, { x + width, y + height, z + depth, 1.f }, normal, color);
        int v23 = OpenGL_AddVertexToMesh(aMeshID, { x - width, y + height, z + depth, 1.f }, normal, color);
        
        OpenGL_AddTriangleToMesh(aMeshID, v22, v20, v21);
        OpenGL_AddTriangleToMesh(aMeshID, v23, v20, v22);
    }
}

void OpenGL_ActivateShader(unsigned int aShaderID)
{
    if(ourOpenGL_Context.myActiveShader != aShaderID)
    {
        ourOpenGL_Context.myActiveShader = aShaderID;
        glUseProgram(ourOpenGL_Context.myActiveShader);
    }
}

void OpenGL_SetShaderUniform(const char* aName, unsigned int aValue)
{
    int location = glGetUniformLocation(ourOpenGL_Context.myActiveShader, aName);
    glUniform1i(location, aValue);
}

void OpenGL_SetShaderUniform(const char* aName, const Vector2f& aVector)
{
    int location = glGetUniformLocation(ourOpenGL_Context.myActiveShader, aName);
    glUniform2f(location, aVector.x, aVector.y);
}

void OpenGL_SetShaderUniform(const char* aName, const Vector3f& aVector)
{
    int location = glGetUniformLocation(ourOpenGL_Context.myActiveShader, aName);
    glUniform3f(location, aVector.x, aVector.y, aVector.z);
}

void OpenGL_SetShaderUniform(const char* aName, const Vector4f& aVector)
{
    int location = glGetUniformLocation(ourOpenGL_Context.myActiveShader, aName);
    glUniform4f(location, aVector.x, aVector.y, aVector.z, aVector.w);
}

void OpenGL_SetShaderUniform(const char* aName, const Matrix& aMatrix)
{
    int location = glGetUniformLocation(ourOpenGL_Context.myActiveShader, aName);
    glUniformMatrix4fv(location, 1, GL_FALSE, aMatrix.myData);   
}
#define GL_LITE_IMPLEMENTATION
#include "gl_lite.h"

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
    
    OpenGL_renderobject myQuad;
    OpenGL_renderobject myCube;
    
    gfx_camera* myCamera;
    unsigned int myActiveShader;
    
    unsigned int myQuadShader;
    unsigned int myCubeShader;
    unsigned int myTextShader;
    
    int myPositionLocation;
    int mySizeLocation;
    int myTextureLocation;
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
    snprintf(fullPath, 100, "data/shaders/opengl/%s", aVertexName);
    DE_File vertexFile = DE_ReadEntireFile(fullPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexFile.myContents, NULL);
    glCompileShader(vertexShader);
    OpenGL_VerifyShader(vertexShader);
    DE_FreeFile(vertexFile);
    
    // Compile our FragmentShader
    snprintf(fullPath, 100, "data/shaders/opengl/%s", aPixelName);
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


void OpenGL_CreateCube()
{
    int meshID = gfx_CreateCubeMesh(0.f, 0.f, 0.f, 1.f, 1.f, 1.f);
    ourOpenGL_Context.myCube = ourOpenGL_Context.myMeshes[meshID];
}

void OpenGL_CreateQuad()
{
    OpenGL_renderobject quad = {};
    
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
    
    // Create a VertexBuffer
    glGenBuffers(1, &quad.myVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, quad.myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Create a IndexBuffer
    glGenBuffers(1, &quad.myElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 
    
    // Setup VertexAttributes (input-layout)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);    
    
    ourOpenGL_Context.myQuad = quad;
}

// gfx_interface-implementation
void gfx_Init(HWND aWindowHandle, int aWindowHeight, int aWindowWidth)
{
    HDC windowDC = GetDC(aWindowHandle);
    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    int suggestedPixelFormatIndex = ChoosePixelFormat(windowDC, &desiredPixelFormat);
    
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(windowDC, suggestedPixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(windowDC, suggestedPixelFormatIndex, &suggestedPixelFormat);
    
    HGLRC openGLRC = wglCreateContext(windowDC);
    bool result = wglMakeCurrent(windowDC, openGLRC);
    ASSERT(result == true);
    
    result = gl_lite_init();
    ASSERT(result == true);
    
    ArrayAlloc(ourOpenGL_Context.myMeshes, 16);
    
    OpenGL_CreateQuad();
    OpenGL_CreateCube();
    
    ourOpenGL_Context.myQuadShader = OpenGL_CreateShader("quad.vx", "quad.px");
    ourOpenGL_Context.myCubeShader = OpenGL_CreateShader("cube.vx", "cube.px");
    ourOpenGL_Context.myTextShader = OpenGL_CreateShader("text.vx", "text.px");
    ourOpenGL_Context.myActiveShader = -1;
    
    ourOpenGL_Context.myCamera = NULL;
}

void gfx_Shutdown()
{
}

void gfx_Viewport(int aX, int aY, int aWidth, int aHeight)
{
    glViewport(aX, aY, aWidth, aHeight);
}

void gfx_ClearColor(float aR, float aG, float aB)
{
    glClearColor(aR, aG, aB, 1.f);
}

void gfx_Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gfx_FinishFrame()
{
    glFinish();
}

unsigned int gfx_CreateTexture(int aWidth, int aHeight, gfxTextureFormat aTextureFormat, void* someTextureData)
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

void gfx_SetCamera(gfx_camera* aCamera)
{
    ourOpenGL_Context.myCamera = aCamera;
}

void gfx_DrawQuad(unsigned int aTextureID, float aX, float aY, float aWidth, float aHeight, const Vector4f& aColor)
{
    ASSERT(ourOpenGL_Context.myCamera != NULL);
    
    Vector2f& screenSize = ourOpenGL_Context.myCamera->myScreenSize;
    
    float normalizedX = aX / screenSize.x;
    float normalizedY = aY / screenSize.y;
    float normalizedWidth = aWidth / screenSize.x;
    float normalizedHeight = aHeight / screenSize.y;
    
    glBindTexture(GL_TEXTURE_2D, aTextureID);
    glUniform2f(ourOpenGL_Context.myPositionLocation, normalizedX, normalizedY);
    glUniform2f(ourOpenGL_Context.mySizeLocation, normalizedWidth, normalizedHeight);
    
    int colorLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "Color");
    glUniform4f(colorLocation, aColor.x, aColor.y, aColor.z, aColor.w);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void gfx_DrawCube(const Matrix& aTransform, const Vector4f& aColor)
{   
    glBindVertexArray(ourOpenGL_Context.myCube.myVertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ourOpenGL_Context.myCube.myElementBufferObject);
    
    int worldLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "World");
    glUniformMatrix4fv(worldLocation, 1, GL_FALSE, aTransform.myData);
    
    int colorAndMetalness = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "ColorAndMetalnessIn");
    glUniform4f(colorAndMetalness, aColor.x, aColor.y, aColor.z, aColor.w);
    
    glDrawElements(GL_TRIANGLES, 32, GL_UNSIGNED_INT, 0);
}

void gfx_DrawMesh(int aMeshID, const Matrix& aTransform)
{
    int worldLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "World");
    glUniformMatrix4fv(worldLocation, 1, GL_FALSE, aTransform.myData);
    
    int colorAndMetalness = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "ColorAndMetalnessIn");
    glUniform4f(colorAndMetalness, 1.f, 1.f, 1.f, 1.f);
    
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh.myVertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.myElementBufferObject);
    glBindVertexArray(mesh.myVertexArrayObject);
    
    glDrawElements(GL_TRIANGLES, mesh.myIndices.myCount, GL_UNSIGNED_INT, 0);
}

void gfx_Begin2D()
{
    if(ourOpenGL_Context.myActiveShader == ourOpenGL_Context.myQuadShader)
        return;
    
    ourOpenGL_Context.myActiveShader = ourOpenGL_Context.myQuadShader;
    glUseProgram(ourOpenGL_Context.myActiveShader);
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(ourOpenGL_Context.myQuad.myVertexArrayObject);
    
    ourOpenGL_Context.myPositionLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "Position");
    ourOpenGL_Context.mySizeLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "Size");
    ourOpenGL_Context.myTextureLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "AlbedoTexture");
    
    glUniform1i(ourOpenGL_Context.myTextureLocation, 0);
}

void gfx_Begin3D()
{
    if(ourOpenGL_Context.myActiveShader != ourOpenGL_Context.myCubeShader)
    {
        ourOpenGL_Context.myActiveShader = ourOpenGL_Context.myCubeShader;
        glUseProgram(ourOpenGL_Context.myActiveShader);
    }
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    ASSERT(ourOpenGL_Context.myCamera != NULL);
    gfx_camera& camera = *ourOpenGL_Context.myCamera;
    
    int projectionLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "Projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, camera.myProjection.myData);
    
    int viewLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "View");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, camera.myInvertedView.myData);
}

void gfx_BeginText()
{
    if(ourOpenGL_Context.myActiveShader == ourOpenGL_Context.myTextShader)
        return;
    
    ourOpenGL_Context.myActiveShader = ourOpenGL_Context.myTextShader;
    glUseProgram(ourOpenGL_Context.myActiveShader);
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(ourOpenGL_Context.myQuad.myVertexArrayObject);
    
    ourOpenGL_Context.myPositionLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "Position");
    ourOpenGL_Context.mySizeLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "Size");
    ourOpenGL_Context.myTextureLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "AlbedoTexture");
    
    glUniform1i(ourOpenGL_Context.myTextureLocation, 0);
}

int gfx_CreateMesh()
{
    OpenGL_renderobject& mesh = ArrayAdd(ourOpenGL_Context.myMeshes);
    ArrayAlloc(mesh.myVertices, 24);
    ArrayAlloc(mesh.myIndices, 32);
    
    return ourOpenGL_Context.myMeshes.myCount - 1;
}

int gfx_AddVertexToMesh(int aMeshID, const Vector4f& aPosition, const Vector4f& aNormal, const Vector4f& aColor)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    OpenGL_cubeVertex& vertex = ArrayAdd(mesh.myVertices);
    
    vertex.myPosition = aPosition;
    vertex.myNormal = aNormal;
    vertex.myColor = aColor;
    
    return mesh.myVertices.myCount - 1;
}

void gfx_AddTriangleToMesh(int aMeshID, int aFirstVertexIndex, int aSecondVertexIndex, int aThirdVertexIndex)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
    ArrayAdd(mesh.myIndices, aFirstVertexIndex);
    ArrayAdd(mesh.myIndices, aSecondVertexIndex);
    ArrayAdd(mesh.myIndices, aThirdVertexIndex);
}

void gfx_FinishMesh(int aMeshID)
{
    OpenGL_renderobject& mesh = ourOpenGL_Context.myMeshes[aMeshID];
 
    // Setup VertexAttributes (input-layout)
    glGenVertexArrays(1, &mesh.myVertexArrayObject);
    glBindVertexArray(mesh.myVertexArrayObject);
    
    // Create a VertexBuffer
    glGenBuffers(1, &mesh.myVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh.myVertices[0]) * mesh.myVertices.myCount, mesh.myVertices.myData, GL_STATIC_DRAW);
    
    // Create a IndexBuffer
    glGenBuffers(1, &mesh.myElementBufferObject);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mesh.myIndices[0]) * mesh.myIndices.myCount, mesh.myIndices.myData, GL_STATIC_DRAW);
 
    
 
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

int gfx_CreateCubeMesh(float x, float y, float z, float r, float g, float b)
{
    int meshID = gfx_CreateMesh();
    
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
    int v0 = gfx_AddVertexToMesh(meshID, p0, normal, color);
    int v1 = gfx_AddVertexToMesh(meshID, p1, normal, color);
    int v2 = gfx_AddVertexToMesh(meshID, p2, normal, color);
    int v3 = gfx_AddVertexToMesh(meshID, p3, normal, color);
    
    gfx_AddTriangleToMesh(meshID, v3, v1, v0);
    gfx_AddTriangleToMesh(meshID, v2, v1, v3);
    
	//4 - 7 (Bottom)
    normal = {0.f, -1.f, 0.f, 0.f};
    int v4 = gfx_AddVertexToMesh(meshID, p4, normal, color);
    int v5 = gfx_AddVertexToMesh(meshID, p5, normal, color);
    int v6 = gfx_AddVertexToMesh(meshID, p6, normal, color);
    int v7 = gfx_AddVertexToMesh(meshID, p7, normal, color);
    
    gfx_AddTriangleToMesh(meshID, v6, v4, v5);
    gfx_AddTriangleToMesh(meshID, v7, v4, v6);
    
	//8 - 11 (Left)
    normal = {-1.f, 0.f, 0.f, 0.f};
    int v8 = gfx_AddVertexToMesh(meshID, p7, normal, color);
    int v9 = gfx_AddVertexToMesh(meshID, p4, normal, color);
    int v10 = gfx_AddVertexToMesh(meshID, p0, normal, color);
    int v11 = gfx_AddVertexToMesh(meshID, p3, normal, color);
    
    gfx_AddTriangleToMesh(meshID, v11, v9, v8);
    gfx_AddTriangleToMesh(meshID, v10, v9, v11);
    
	//12 - 15 (Right)
    normal = {1.f, 0.f, 0.f, 0.f};
    int v12 = gfx_AddVertexToMesh(meshID, p6, normal, color);
    int v13 = gfx_AddVertexToMesh(meshID, p5, normal, color);
    int v14 = gfx_AddVertexToMesh(meshID, p1, normal, color);
    int v15 = gfx_AddVertexToMesh(meshID, p2, normal, color);
    
    gfx_AddTriangleToMesh(meshID, v14, v12, v13);
    gfx_AddTriangleToMesh(meshID, v15, v12, v14);
    
	//16 - 19 (Front)
    normal = {0.f, 0.f, 1.f, 0.f};
    int v16 = gfx_AddVertexToMesh(meshID, p4, normal, color);
    int v17 = gfx_AddVertexToMesh(meshID, p5, normal, color);
    int v18 = gfx_AddVertexToMesh(meshID, p1, normal, color);
    int v19 = gfx_AddVertexToMesh(meshID, p0, normal, color);
    
    gfx_AddTriangleToMesh(meshID, v19, v17, v16);
    gfx_AddTriangleToMesh(meshID, v18, v17, v19);
    
	//20 - 23 (Back)
    normal = {0.f, 0.f, -1.f, 0.f};
    int v20 = gfx_AddVertexToMesh(meshID, p7, normal, color);
    int v21 = gfx_AddVertexToMesh(meshID, p6, normal, color);
    int v22 = gfx_AddVertexToMesh(meshID, p2, normal, color);
    int v23 = gfx_AddVertexToMesh(meshID, p4, normal, color);
    
    gfx_AddTriangleToMesh(meshID, v22, v20, v21);
    gfx_AddTriangleToMesh(meshID, v23, v20, v22);
    
    gfx_FinishMesh(meshID);
    return meshID;
}
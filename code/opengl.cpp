#define GL_LITE_IMPLEMENTATION
#include "gl_lite.h"

struct OpenGL_modelData
{
    Matrix myMatrix;
    Vector4f myColor;
};

struct OpenGL_cubeVertex
{
    Vector4f pos;
    Vector4f norm;
};

struct OpenGL_renderobject
{  
    unsigned int myVertexArrayObject;
    unsigned int myVertexBufferObject;
    unsigned int myElementBufferObject;
};

struct OpenGL_context
{
    OpenGL_renderobject myQuad;
    OpenGL_renderobject myCube;
    
    Matrix myView;
    Matrix myProjection;
    unsigned int myActiveShader;
    
    unsigned int myQuadShader;
    unsigned int myCubeShader;
    
    GrowingArray<OpenGL_modelData> myModels;
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
    const char* vertexData = DE_ReadEntireFile(fullPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexData, NULL);
    glCompileShader(vertexShader);
    OpenGL_VerifyShader(vertexShader);
    free((void*)vertexData);
    
    // Compile our FragmentShader
    snprintf(fullPath, 100, "data/shaders/opengl/%s", aPixelName);
    const char* pixelData = DE_ReadEntireFile(fullPath);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &pixelData, NULL);
    glCompileShader(fragmentShader);
    OpenGL_VerifyShader(fragmentShader);
    free((void*)pixelData);
    
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
    // Create our Vertices
    float width = 0.5f;
    float height = 0.5f;
    float depth = 0.5f;
    
    Vector4f up = {0.f, 1.f, 0.f, 0.f};
    Vector4f down = {0.f, -1.f, 0.f, 0.f};
    
    Vector4f left = {-1.f, 0.f, 0.f, 0.f};
    Vector4f right = {1.f, 0.f, 0.f, 0.f};
    
    Vector4f forward = {0.f, 0.f, 1.f, 0.f};
    Vector4f back = {0.f, 0.f, -1.f, 0.f};
    
    OpenGL_cubeVertex vertices[24];
    //0 - 3 (Top)
	vertices[0] = { -width, height, -depth, 1.f, up };
	vertices[1] = { width, height, -depth, 1.f, up };
	vertices[2] = { width, height, depth, 1.f, up };
	vertices[3] = { -width, height, depth, 1.f, up };

	//4 - 7 (Bottom)
	vertices[4] = { -width, -height, -depth, 1.f, down };
	vertices[5] = { width, -height, -depth, 1.f, down };
	vertices[6] = { width, -height, depth, 1.f, down };
	vertices[7] = { -width, -height, depth, 1.f, down };

	//8 - 11 (Left)
	vertices[8] = { -width, -height, depth, 1.f, left };
	vertices[9] = { -width, -height, -depth, 1.f, left };
	vertices[10] = { -width, height, -depth, 1.f, left };
	vertices[11] = { -width, height, depth, 1.f, left };

	//12 - 15 (Right)
	vertices[12] = { width, -height, depth, 1.f, right };
	vertices[13] = { width, -height, -depth, 1.f, right };
	vertices[14] = { width, height, -depth, 1.f, right };
	vertices[15] = { width, height, depth, 1.f, right };

	//16 - 19 (Front)
	vertices[16] = { -width, -height, -depth, 1.f, forward };
	vertices[17] = { width, -height, -depth, 1.f, forward };
	vertices[18] = { width, height, -depth, 1.f, forward };
	vertices[19] = { -width, height, -depth, 1.f, forward };

	//20 - 23 (Back)
	vertices[20] = { -width, -height, depth, 1.f, back };
	vertices[21] = { width, -height, depth, 1.f, back };
	vertices[22] = { width, height, depth, 1.f, back };
	vertices[23] = { -width, height, depth, 1.f, back };
    
	DWORD indices[36];
	//Top
	indices[0] = 3;
	indices[1] = 1;
	indices[2] = 0;

	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	//Bottom
	indices[6] = 6;
	indices[7] = 4;
	indices[8] = 5;

	indices[9] = 7;
	indices[10] = 4;
	indices[11] = 6;

	//Left
	indices[12] = 11;
	indices[13] = 9;
	indices[14] = 8;

	indices[15] = 10;
	indices[16] = 9;
	indices[17] = 11;

	//Right
	indices[18] = 14;
	indices[19] = 12;
	indices[20] = 13;

	indices[21] = 15;
	indices[22] = 12;
	indices[23] = 14;

	//Front
	indices[24] = 19;
	indices[25] = 17;
	indices[26] = 16;

	indices[27] = 18;
	indices[28] = 17;
	indices[29] = 19;

	//Back
	indices[30] = 22;
	indices[31] = 20;
	indices[32] = 21;

	indices[33] = 23;
	indices[34] = 20;

 
    OpenGL_renderobject cube = {};
    
    glGenVertexArrays(1, &cube.myVertexArrayObject);
    glBindVertexArray(cube.myVertexArrayObject);
    
    
    // Create a VertexBuffer
    glGenBuffers(1, &cube.myVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, cube.myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Create a IndexBuffer
    glGenBuffers(1, &cube.myElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 
    
    // Setup VertexAttributes (input-layout)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    ourOpenGL_Context.myCube = cube;
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
         size,  size, 0.f,   1.f,  0.f,  0.f,    1.f, 0.0f,  // topright
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
    
    glEnable(GL_DEPTH_TEST);
    OpenGL_CreateQuad();
    OpenGL_CreateCube();
    
    ourOpenGL_Context.myQuadShader = OpenGL_CreateShader("quad.vx", "quad.px");
    ourOpenGL_Context.myCubeShader = OpenGL_CreateShader("cube.vx", "cube.px");
    
    ArrayAlloc(ourOpenGL_Context.myModels, 12);
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

unsigned int gfx_CreateTexture(int aWidth, int aHeight, bool aUseAlpha, void* someTextureData)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, aWidth, aHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, someTextureData);
    
    return texture;
}

void gfx_BindTexture(unsigned int aTextureID, unsigned int aTextureSlot, const char* aShaderTextureName)
{
    glUniform1i(glGetUniformLocation(ourOpenGL_Context.myActiveShader, aShaderTextureName), aTextureSlot);
    
    glActiveTexture(GL_TEXTURE0 + aTextureSlot);
    glBindTexture(GL_TEXTURE_2D, aTextureID);
}

void gfx_CommitConstantData(const gfx_camera& aCamera)
{
    ourOpenGL_Context.myProjection = aCamera.myProjection;
    ourOpenGL_Context.myView = aCamera.myInvertedView;
    
    int projectionLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "Projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, ourOpenGL_Context.myProjection.myData);
    
    int viewLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "View");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, ourOpenGL_Context.myView.myData);
}

void gfx_Begin2D()
{
    glUseProgram(ourOpenGL_Context.myQuadShader);
    ourOpenGL_Context.myActiveShader = ourOpenGL_Context.myQuadShader;
}

void gfx_DrawQuad()
{
    glBindVertexArray(ourOpenGL_Context.myQuad.myVertexArrayObject);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void gfx_Begin3D()
{
    glUseProgram(ourOpenGL_Context.myCubeShader);
    ourOpenGL_Context.myActiveShader = ourOpenGL_Context.myCubeShader;
}

void gfx_DrawCube(const Matrix& aTransform)
{   
    ArrayAdd(ourOpenGL_Context.myModels, {aTransform, 1.f, 1.f, 1.f, 1.f});
}

void gfx_DrawColoredCube(const Matrix& aTransform, const Vector4f& aColor)
{    
    ArrayAdd(ourOpenGL_Context.myModels, {aTransform, aColor});
}

void gfx_DrawModels()
{
    glUseProgram(ourOpenGL_Context.myCubeShader);
    ourOpenGL_Context.myActiveShader = ourOpenGL_Context.myCubeShader;
    
    for(int i = 0; i < ourOpenGL_Context.myModels.myCount; ++i)
    {
        const OpenGL_modelData& data = ourOpenGL_Context.myModels[i];
        
        int worldLocation = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "World");
        glUniformMatrix4fv(worldLocation, 1, GL_FALSE, data.myMatrix.myData);
        
        int colorAndMetalness = glGetUniformLocation(ourOpenGL_Context.myActiveShader, "ColorAndMetalnessIn");
        glUniform4f(colorAndMetalness, data.myColor.x, data.myColor.y, data.myColor.z, data.myColor.w);
        
        glBindVertexArray(ourOpenGL_Context.myCube.myVertexArrayObject);
        glDrawElements(GL_TRIANGLES, 32, GL_UNSIGNED_INT, 0);    
    }
    
    ArrayClear(ourOpenGL_Context.myModels);
}
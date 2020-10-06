void OpenGL_Init();
void OpenGL_Viewport(int aX, int aY, int aWidth, int aHeight);
void OpenGL_ClearColor(float aR, float aG, float aB);
void OpenGL_Clear();
void OpenGL_FinishFrame();

enum gfxTextureFormat
{
    SINGLE_CHANNEL,
    RGB,
    RGBA,
};
unsigned int OpenGL_CreateTexture(int aWidth, int aHeight, gfxTextureFormat aTextureFormat, void* someTextureData);

void OpenGL_DrawMesh(int aMeshID);

int OpenGL_CreateMesh();
int OpenGL_CreateQuadMesh();
void OpenGL_ClearMesh(int aMeshID);

int OpenGL_AddVertexToMesh(int aMeshID, const Vector4f& aPosition, const Vector4f& aNormal, const Vector4f& aColor);
void OpenGL_AddTriangleToMesh(int aMeshID, int aFirstVertexIndex, int aSecondVertexIndex, int aThirdVertexIndex);

enum OpenGL_CubeMeshFlags
{
    LEFT = 1 << 0,
    RIGHT = 1 << 1,
    TOP =  1 << 2,
    BOTTOM = 1 << 3,
    FRONT = 1 << 4,
    BACK = 1 << 5,
    ALL = LEFT | RIGHT | TOP | BOTTOM | FRONT | BACK,
};
void  OpenGL_CreateCubeMesh(int aMeshID, float x, float y, float z, float r, float g, float b, int someFlags);

void OpenGL_FinishMesh(int aMeshID);



unsigned int OpenGL_CreateShader(const char* aVertexShaderFileName, const char* aPixelShaderFileName);
void OpenGL_ActivateShader(unsigned int aShaderID);
void OpenGL_SetShaderUniform(const char* aName, unsigned int);
void OpenGL_SetShaderUniform(const char* aName, const Vector2f& aVector);
void OpenGL_SetShaderUniform(const char* aName, const Vector3f& aVector);
void OpenGL_SetShaderUniform(const char* aName, const Vector4f& aVector);
void OpenGL_SetShaderUniform(const char* aName, const Matrix& aMatrix);
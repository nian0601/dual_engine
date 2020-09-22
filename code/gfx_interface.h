struct gfx_camera
{
    Matrix myProjection;
    Matrix myView;
    Matrix myInvertedView;
    Vector2f myWindowSize;
    Vector2f myScreenSize;
};


void gfx_Init(HWND aWindowHandle, int aWindowWidth, int aWindowHeight);
void gfx_Shutdown();
void gfx_Viewport(int aX, int aY, int aWidth, int aHeight);
void gfx_ClearColor(float aR, float aG, float aB);
void gfx_Clear();
void gfx_FinishFrame();

enum gfxTextureFormat
{
    SINGLE_CHANNEL,
    RGB,
    RGBA,
};
unsigned int gfx_CreateTexture(int aWidth, int aHeight, gfxTextureFormat aTextureFormat, void* someTextureData);

void gfx_SetCamera(gfx_camera* aCamera);

void gfx_DrawQuad(unsigned int aTextureID, float aX, float aY, float aWidth, float aHeight, const Vector4f& aColor);
void gfx_DrawCube(const Matrix& aTransform, const Vector4f& aColor);
void gfx_DrawMesh(int aMeshID, const Matrix& aTransform);


void gfx_Begin2D();
void gfx_Begin3D();
void gfx_BeginText();


int gfx_CreateMesh();
void gfx_ClearMesh(int aMeshID);

int gfx_AddVertexToMesh(int aMeshID, const Vector4f& aPosition, const Vector4f& aNormal, const Vector4f& aColor);
void gfx_AddTriangleToMesh(int aMeshID, int aFirstVertexIndex, int aSecondVertexIndex, int aThirdVertexIndex);

void  gfx_CreateCubeMesh(int aMeshID, float x, float y, float z, float r, float g, float b);

void gfx_FinishMesh(int aMeshID);
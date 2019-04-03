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

void gfx_DrawQuad(unsigned int aTextureID, float aX1, float aY1, float aX2, float aY2);

void gfx_DrawQuads();

void gfx_DrawCube(const Matrix& aTransform);
void gfx_DrawColoredCube(const Matrix& aTransform, const Vector4f& aColor);

void gfx_DrawModels();
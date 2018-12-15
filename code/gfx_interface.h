void gfx_Init(HWND aWindowHandle, int aWindowWidth, int aWindowHeight);
void gfx_Shutdown();
void gfx_Viewport(int aX, int aY, int aWidth, int aHeight);
void gfx_ClearColor(float aR, float aG, float aB);
void gfx_Clear();
void gfx_FinishFrame();

unsigned int gfx_CreateTexture(int aWidth, int aHeight, bool aUseAlpha, void* someTextureData);
void gfx_BindTexture(unsigned int aTextureID, unsigned int aTextureSlot, const char* aShaderTextureName);

void gfx_SetProjection(const Matrix& aProjection);
void gfx_SetView(const Matrix& aView);
void gfx_CommitConstantData();

void gfx_Begin2D();
void gfx_DrawQuad();

void gfx_Begin3D();
void gfx_DrawCube(const Matrix& aTransform);
void gfx_DrawColoredCube(const Matrix& aTransform, const Vector4f& aColor);
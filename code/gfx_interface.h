void gfx_Init(HWND aWindowHandle, int aWindowWidth, int aWindowHeight);
void gfx_Shutdown();
void gfx_Viewport(int aX, int aY, int aWidth, int aHeight);
void gfx_ClearColor(float aR, float aG, float aB);

unsigned int gfx_CreateShader(const char* aVertexName, const char* aPixelName);
unsigned int gfx_CreateHardcodedShader();
void gfx_BindShader(unsigned int aShaderID);
void gfx_ShaderConstanti(unsigned int aShaderID, const char* aConstantName, int aValue);

void gfx_SetProjection(const Matrix& aProjection);
void gfx_SetView(const Matrix& aView);
void gfx_CommitConstantData();

unsigned int gfx_CreateTexture(int aWidth, int aHeight, bool aUseAlpha, void* someTextureData);
void gfx_BindTexture(unsigned int aTextureID, unsigned int aTextureSlot);

void gfx_DrawQuad();
void gfx_DrawCube(const Matrix& aTransform);

void gfx_Clear();
void gfx_FinishFrame();
void gfx_Init(HWND aWindowHandle);
void gfx_Viewport(int aX, int aY, int aWidth, int aHeight);
void gfx_ClearColor(float aR, float aG, float aB);

unsigned int gfx_CreateShader(const char* aVertexShaderData, const char* aFragmentShaderData);
void gfx_BindShader(unsigned int aShaderID);
void gfx_ShaderConstanti(unsigned int aShaderID, const char* aConstantName, int aValue);

unsigned int gfx_CreateTexture(int aWidth, int aHeight, bool aUseAlpha, void* someTextureData);
void gfx_BindTexture(unsigned int aTextureID, unsigned int aTextureSlot);

void gfx_DrawQuad();

void gfx_Clear();
void gfx_FinishFrame();
struct DX_modelData
{
    Matrix myMatrix;
    Vector4f myColor;
};

struct DX_quadData
{
    Vector2f myPosition;
    Vector2f mySize;
    Vector4f myColor;
    unsigned int myTextureID;
};

struct DX_quadVertex
{
    Vector3f pos;
    Vector2f tex;
};

struct DX_cubeVertex
{
    Vector4f pos;
    Vector4f norm;
};

struct DX_3dConstants
{
    Matrix myView;
    Matrix myProjection;
};

struct DX_shader
{
    ID3D11VertexShader* myVertexShader;
    ID3D11PixelShader* myPixelShader;
    ID3D10Blob* myVertexShaderBuffer;
    ID3D10Blob* myPixelShaderBuffer;
    ID3D11InputLayout* myInputLayout;
};

struct DX_texture
{
    ID3D11Texture2D* myTexture;
    ID3D11ShaderResourceView* myShaderResource;
};

struct DX_constantBuffer
{
    ID3D11Buffer* myBuffer;
    unsigned int myBufferSize;
    int myBufferIndex;
};

struct DX_renderobject
{ 
    ID3D11Buffer* myVertexBuffer;
    ID3D11Buffer* myIndexBuffer;
    DX_constantBuffer myConstantBuffer;
};

enum DX_BlendState
{
    NO_BLEND,
    ALPHA_BLEND,
    
    //
    BLEND_COUNT
};

enum DX_DepthState
{
    ENABLED,
    DISABLED,
    NO_READ_NO_WRITE,
    
    //
    DEPTH_COUNT
};

struct DX_context
{
    IDXGISwapChain* mySwapChain;
    ID3D11Device* myDevice;
    ID3D11DeviceContext* myContext;
    ID3D11RenderTargetView* myBackBuffer;
    ID3D11DepthStencilView* myDepthStencil;
    ID3D11Texture2D* myDepthTexture;
    
    float myClearColor[4];
    
    DX_shader myQuadShader;
    DX_shader myTextShader;
    DX_renderobject myQuad;
    
    GrowingArray<DX_texture> myTextures;

    DX_constantBuffer myConstantBuffer;
    DX_3dConstants my3DConstants;
    gfx_camera* myCamera;
    
    DX_shader myCubeShader;
    DX_renderobject myCube;
    
    ID3D11SamplerState* mySamplerState;
    ID3D11RasterizerState* myRasterizerState;
    ID3D11DepthStencilState* myDepthStates[DEPTH_COUNT];
    ID3D11BlendState* myBlendStates[BLEND_COUNT];
};
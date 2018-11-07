
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

const char *vertexShaderSource = "\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 Pos : SV_POSITION;\n"
"    float2 Tex : TEXCOORD;\n"
"};\n"
"VS_OUTPUT VS(float4 inPos : POSITION, float2 inTex : TEXCOORD)\n"
"{\n"
"    VS_OUTPUT output;\n"
"    output.Pos = inPos;\n"
"    output.Tex = inTex;\n"
"    return output;\n"
"}";

const char *pixelShaderSource = "\n"
"SamplerState LinearSampler : register(s0);\n"
"Texture2D AlbedoTexture : register(t0);\n"
"struct VS_OUTPUT\n"
"{\n"
"    float4 Pos : POSITION;\n"
"    float2 Tex : TEXCOORD;\n"
"};\n"
"float4 PS(VS_OUTPUT input) : SV_TARGET\n"
"{\n"
"   return AlbedoTexture.Sample(LinearSampler, input.Tex);\n"
"   //return float4(0.f, 1.f, 0.f, 1.f);\n"
"}";

#include <d3d11.h>
#include <D3DCompiler.h>

struct Vector3f
{
    float x;
    float y;
    float z;
};

struct Vector2f
{
    float x;
    float y;
};

struct Vertex
{
    Vector3f pos;
    Vector2f tex;
};

struct dirextx_quadShader
{
    ID3D11VertexShader* myVertexShader;
    ID3D11PixelShader* myPixelShader;
    ID3D10Blob* myVertexShaderBuffer;
    ID3D10Blob* myPixelShaderBuffer;
    ID3D11InputLayout* myInputLayout;
};

struct directx_texture
{
    ID3D11Texture2D* myTexture;
    ID3D11ShaderResourceView* myShaderResource;
};

struct directx_renderobject
{ 
    ID3D11Buffer* myVertexBufffer;
    ID3D11Buffer* myIndexBuffer;
};

struct directx_context
{
    IDXGISwapChain* mySwapChain;
    ID3D11Device* myDevice;
    ID3D11DeviceContext* myContext;
    ID3D11RenderTargetView* myRenderTarget;
    
    float myClearColor[4];
    
    dirextx_quadShader myQuadShader;
    directx_renderobject myQuad;
    directx_texture myTexture;
    
    ID3D11SamplerState* mySamplerState;
    ID3D11RasterizerState* myRasterizerState;
};

static directx_context ourDirectXContext;

void directxCreateQuad()
{
    directx_renderobject& quad = ourDirectXContext.myQuad;
    
    Vertex v[] =
    {
        // Bottom Left
        {-0.5f, -0.5f, 0.0f, 0.f, 1.f},
        
        // Bottomright
        {0.5f, -0.5f, 0.5f, 1.f, 1.f},

        // Top Left
        { -0.5f, 0.5f, 0.5f, 0.f, 0.f},
        
        // Top Right
        { 0.5f,  0.5f, 0.5f, 1.f, 0.f},
    };
    
    D3D11_BUFFER_DESC vertexDesc = {};
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.ByteWidth = sizeof(Vertex) * 4;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = 0;
    vertexDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = v;
    HRESULT result = ourDirectXContext.myDevice->CreateBuffer(
        &vertexDesc, 
        &vertexData, 
        &quad.myVertexBufffer);
    ASSERT(result == S_OK);
    
    DWORD indices[] =
    {
        0, 2, 1,
        1, 2, 3,
    };
    
    D3D11_BUFFER_DESC indexDesc = {};
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexDesc.CPUAccessFlags = 0;
    indexDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;
    result = ourDirectXContext.myDevice->CreateBuffer(
        &indexDesc, 
        &indexData, 
        &quad.myIndexBuffer);
    ASSERT(result == S_OK);
    
}

void directXCreateSamplerState()
{
    D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    HRESULT result = ourDirectXContext.myDevice->CreateSamplerState(
        &samplerDesc, 
        &ourDirectXContext.mySamplerState);
    ASSERT(result == S_OK);
    
    ourDirectXContext.myContext->PSSetSamplers(0, 1, &ourDirectXContext.mySamplerState);
}

void directXCreateRasterizerState()
{
    D3D11_RASTERIZER_DESC desc = {};
	desc.FrontCounterClockwise = false;
	desc.DepthBias = false;
	desc.DepthBiasClamp = 0;
	desc.SlopeScaledDepthBias = 0;
	desc.DepthClipEnable = false;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;
    desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
    
    ourDirectXContext.myDevice->CreateRasterizerState(&desc, &ourDirectXContext.myRasterizerState);
}

void gfx_Init(HWND aWindowHandle, int aWindowWidth, int aWindowHeight)
{
    DXGI_MODE_DESC bufferDesc = {};
    bufferDesc.Width = aWindowWidth;
    bufferDesc.Height = aWindowHeight;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc = bufferDesc;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = aWindowHandle;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    
    HRESULT result = D3D11CreateDeviceAndSwapChain(
        NULL, 
        D3D_DRIVER_TYPE_HARDWARE, 
        NULL, 
        NULL, 
        NULL, 
        NULL, 
        D3D11_SDK_VERSION, 
        &swapChainDesc, 
        &ourDirectXContext.mySwapChain, 
        &ourDirectXContext.myDevice, 
        NULL, 
        &ourDirectXContext.myContext);
    
    ASSERT(result == S_OK);
    
    ID3D11Texture2D* backBuffer;
    result = ourDirectXContext.mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    ASSERT(result == S_OK);
    result = ourDirectXContext.myDevice->CreateRenderTargetView(
        backBuffer, 
        NULL, 
        &ourDirectXContext.myRenderTarget);
    
    ASSERT(result == S_OK);
    
    backBuffer->Release();
    
    ourDirectXContext.myContext->OMSetRenderTargets(1, &ourDirectXContext.myRenderTarget, NULL);
    
    directxCreateQuad();
    directXCreateSamplerState();
}

void gfx_Shutdown()
{
    ourDirectXContext.mySwapChain->Release();
    ourDirectXContext.myDevice->Release();
    ourDirectXContext.myContext->Release();
    
    directx_renderobject& quad = ourDirectXContext.myQuad;
    quad.myVertexBufffer->Release();
    
    dirextx_quadShader& shader = ourDirectXContext.myQuadShader;
    shader.myVertexShader->Release();
    shader.myPixelShader->Release();
    shader.myVertexShaderBuffer->Release();
    shader.myPixelShaderBuffer->Release();
    shader.myInputLayout->Release();
}

void gfx_Viewport(int aX, int aY, int aWidth, int aHeight)
{
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = aX;
    viewport.TopLeftY = aY;
    viewport.Width = aWidth;
    viewport.Height = aHeight;
    
    ourDirectXContext.myContext->RSSetViewports(1, &viewport);
}

void gfx_ClearColor(float aR, float aG, float aB)
{
    ourDirectXContext.myClearColor[0] = aR;
    ourDirectXContext.myClearColor[1] = aG;
    ourDirectXContext.myClearColor[2] = aB;
    ourDirectXContext.myClearColor[3] = 1;
}


unsigned int gfx_CreateShader(const char* aVertexShaderData, const char* aPixelShaderData)
{
    dirextx_quadShader& shader = ourDirectXContext.myQuadShader;
    
    ID3DBlob* errorBlob = NULL;
    
    HRESULT result = D3DCompile(
        aVertexShaderData,
        strlen(aVertexShaderData) * sizeof(char),
        NULL,
        NULL,
        NULL,
        "VS",
        "vs_5_0",
        0,
        0,
        &shader.myVertexShaderBuffer,
        &errorBlob);
    
    if(result != S_OK)
    {
        const char* errorMsg = NULL;
        if(errorBlob)
            errorMsg = (const char*)errorBlob->GetBufferPointer();
        
        ASSERT(false);
    }
    
    result = D3DCompile(
        aPixelShaderData,
        strlen(aPixelShaderData) * sizeof(char),
        NULL,
        NULL,
        NULL,
        "PS",
        "ps_5_0",
        0,
        0,
        &shader.myPixelShaderBuffer,
        &errorBlob);
    if(result != S_OK)
    {
        const char* errorMsg = NULL;
        if(errorBlob)
            errorMsg = (const char*)errorBlob->GetBufferPointer();
        
        ASSERT(false);
    }
    
    if(errorBlob)
        errorBlob->Release();
    
    result = ourDirectXContext.myDevice->CreateVertexShader(
        shader.myVertexShaderBuffer->GetBufferPointer(),
        shader.myVertexShaderBuffer->GetBufferSize(),
        NULL,
        &shader.myVertexShader);
    ASSERT(result == S_OK);
    
    result = ourDirectXContext.myDevice->CreatePixelShader(
        shader.myPixelShaderBuffer->GetBufferPointer(),
        shader.myPixelShaderBuffer->GetBufferSize(),
        NULL,
        &shader.myPixelShader);
    ASSERT(result == S_OK);
    
    
    
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE(layout);
    
    result = ourDirectXContext.myDevice->CreateInputLayout(
        layout,
        numElements,
        shader.myVertexShaderBuffer->GetBufferPointer(),
        shader.myVertexShaderBuffer->GetBufferSize(),
        &shader.myInputLayout);
        
        
    return 0;
}

unsigned int gfx_CreateHardcodedShader()
{
    return gfx_CreateShader(vertexShaderSource, pixelShaderSource);
}

void gfx_BindShader(unsigned int aShaderID)
{
    ourDirectXContext.myContext->VSSetShader(ourDirectXContext.myQuadShader.myVertexShader, 0, 0);
    ourDirectXContext.myContext->PSSetShader(ourDirectXContext.myQuadShader.myPixelShader, 0, 0);
    ourDirectXContext.myContext->IASetInputLayout(ourDirectXContext.myQuadShader.myInputLayout);
    ourDirectXContext.myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void gfx_ShaderConstanti(unsigned int aShaderID, const char* aConstantName, int aValue)
{
}


unsigned int gfx_CreateTexture(int aWidth, int aHeight, bool aUseAlpha, void* someTextureData)
{
    //HRESULT result = DirectX::CreateWICTextureFromMemory(
    //    ourDirectXContext.myDevice,
    //    ourDirectXContext.myContext,
    //    (uint8_t*)someTextureData,
    //    sizeof(unsigned char) * aWidth * aHeight,
    //    &ourDirectXContext.myTexture.myResource,
    //   &ourDirectXContext.myTexture.myShaderResource);
    
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = aWidth;
    textureDesc.Height = aHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    
    int numComponents;
    if(aUseAlpha)
    {
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        numComponents = 4;
    }
    else
    {
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        numComponents = 3;
    }
     
    
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA textureData = {};
    textureData.pSysMem = someTextureData;
    textureData.SysMemPitch = sizeof(unsigned char) * numComponents  * aWidth;
    textureData.SysMemSlicePitch = 0;
    
    HRESULT result = ourDirectXContext.myDevice->CreateTexture2D(
        &textureDesc,
        &textureData,
        &ourDirectXContext.myTexture.myTexture);
    ASSERT(result == S_OK);
    
    result = ourDirectXContext.myDevice->CreateShaderResourceView(
        ourDirectXContext.myTexture.myTexture,
        NULL,
        &ourDirectXContext.myTexture.myShaderResource);
    ASSERT(result == S_OK);
    
    return 0;
}

void gfx_BindTexture(unsigned int aTextureID, unsigned int aTextureSlot)
{
    ourDirectXContext.myContext->PSSetShaderResources(0, 1, &ourDirectXContext.myTexture.myShaderResource);
}


void gfx_DrawQuad()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    
    ourDirectXContext.myContext->IASetVertexBuffers(
        0, 
        1, 
        &ourDirectXContext.myQuad.myVertexBufffer, 
        &stride, 
        &offset);
    
    ourDirectXContext.myContext->IASetIndexBuffer(
        ourDirectXContext.myQuad.myIndexBuffer, 
        DXGI_FORMAT_R32_UINT, 
        0);
    
    ourDirectXContext.myContext->DrawIndexed(6, 0, 0);
}


void gfx_Clear()
{
    ourDirectXContext.myContext->ClearRenderTargetView(
        ourDirectXContext.myRenderTarget, 
        ourDirectXContext.myClearColor);
}

void gfx_FinishFrame()
{
    ourDirectXContext.mySwapChain->Present(0, 0);
}
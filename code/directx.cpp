#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <D3DCompiler.h>

#include "directx.h"

static DX_context ourDirectXContext;

void DX_CreateConstantBuffer(DX_constantBuffer& aBuffer)
{
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = aBuffer.myBufferSize;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0.;
    bufferDesc.StructureByteStride = 0;
    
    HRESULT result = ourDirectXContext.myDevice->CreateBuffer(
        &bufferDesc,
        NULL,
        &aBuffer.myBuffer);
    ASSERT(result == S_OK);
}

void DX_CreateQuad()
{
    DX_renderobject& quad = ourDirectXContext.myQuad;
    
    DX_quadVertex v[] =
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
    vertexDesc.ByteWidth = sizeof(DX_quadVertex) * 4;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = 0;
    vertexDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = v;
    HRESULT result = ourDirectXContext.myDevice->CreateBuffer(
        &vertexDesc, 
        &vertexData, 
        &quad.myVertexBuffer);
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

    quad.myConstantBuffer = {};
}

void DX_CreateCube()
{    
    float width = 0.5f;
    float height = 0.5f;
    float depth = 0.5f;
    
    Vector4f up = {0.f, 1.f, 0.f, 0.f};
    Vector4f down = {0.f, -1.f, 0.f, 0.f};
    
    Vector4f left = {-1.f, 0.f, 0.f, 0.f};
    Vector4f right = {1.f, 0.f, 0.f, 0.f};
    
    Vector4f forward = {0.f, 0.f, 1.f, 0.f};
    Vector4f back = {0.f, 0.f, -1.f, 0.f};
    
    DX_cubeVertex vertices[24];
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
	indices[35] = 22;
    
    D3D11_BUFFER_DESC vertexDesc = {};
    vertexDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexDesc.ByteWidth = sizeof(DX_cubeVertex) * 24;
    vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexDesc.CPUAccessFlags = 0;
    vertexDesc.MiscFlags = 0;
 
    DX_renderobject& cube = ourDirectXContext.myCube;
    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices;
    HRESULT result = ourDirectXContext.myDevice->CreateBuffer(
        &vertexDesc, 
        &vertexData, 
        &cube.myVertexBuffer);
    ASSERT(result == S_OK);
    
    D3D11_BUFFER_DESC indexDesc = {};
    indexDesc.Usage = D3D11_USAGE_DEFAULT;
    indexDesc.ByteWidth = sizeof(DWORD) * 36;
    indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexDesc.CPUAccessFlags = 0;
    indexDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices;
    result = ourDirectXContext.myDevice->CreateBuffer(
        &indexDesc, 
        &indexData, 
        &cube.myIndexBuffer);
    ASSERT(result == S_OK);
    
    cube.myConstantBuffer.myBufferSize = sizeof(Matrix);
    cube.myConstantBuffer.myBufferIndex = 1;
    DX_CreateConstantBuffer(cube.myConstantBuffer);
}

void DX_CreateSamplerState()
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

void DX_CreateRasterizerState()
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
    
    ourDirectXContext.myContext->RSSetState(ourDirectXContext.myRasterizerState);
}

void DX_CreateDepthState()
{
    D3D11_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = true;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    HRESULT result = ourDirectXContext.myDevice->CreateDepthStencilState(
        &depthDesc,
        &ourDirectXContext.myDepthState);
    ASSERT(result == S_OK);
    
    ourDirectXContext.myContext->OMSetDepthStencilState(ourDirectXContext.myDepthState, 1);
}

void DX_CreateSwapChain(HWND aWindowHandle, int aWindowWidth, int aWindowHeight)
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
}

void DX_CreateBackbuffer()
{
    ID3D11Texture2D* backBuffer;
    HRESULT result = ourDirectXContext.mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    ASSERT(result == S_OK);
    result = ourDirectXContext.myDevice->CreateRenderTargetView(
        backBuffer, 
        NULL, 
        &ourDirectXContext.myBackBuffer);
    ASSERT(result == S_OK);
    
    backBuffer->Release();
}

void DX_CreateDepthBuffer(int aWidth, int aHeight)
{
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = aWidth;
    texDesc.Height = aHeight;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_D32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE(0);
    texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    HRESULT result = ourDirectXContext.myDevice->CreateTexture2D(
        &texDesc, 
        NULL, 
        &ourDirectXContext.myDepthTexture);
    
    D3D11_DEPTH_STENCIL_VIEW_DESC stencilDesc = {};
    stencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
    stencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    stencilDesc.Texture2D.MipSlice = 0;
    
    result = ourDirectXContext.myDevice->CreateDepthStencilView(
        ourDirectXContext.myDepthTexture,
        &stencilDesc,
        &ourDirectXContext.myDepthStencil);
    ASSERT(result == S_OK);
}
 
void gfx_Init(HWND aWindowHandle, int aWindowWidth, int aWindowHeight)
{
    DX_CreateSwapChain(aWindowHandle, aWindowWidth, aWindowHeight);
    DX_CreateBackbuffer();
    DX_CreateDepthBuffer(aWindowWidth, aWindowHeight);
    
    
    ourDirectXContext.myContext->OMSetRenderTargets(1, &ourDirectXContext.myBackBuffer, ourDirectXContext.myDepthStencil);

    DX_CreateSamplerState();
    DX_CreateDepthState();
    DX_CreateRasterizerState();
    
    DX_CreateQuad();
    DX_CreateCube();
    
    
    ourDirectXContext.myConstantBuffer.myBufferSize = sizeof(ourDirectXContext.my3DConstants);
    ourDirectXContext.myConstantBuffer.myBufferIndex = 0;
    DX_CreateConstantBuffer(ourDirectXContext.myConstantBuffer);
}

void gfx_Shutdown()
{
    ourDirectXContext.mySwapChain->Release();
    ourDirectXContext.myDevice->Release();
    ourDirectXContext.myContext->Release();
    
    DX_renderobject& quad = ourDirectXContext.myQuad;
    quad.myVertexBuffer->Release();
    
    DX_shader& shader = ourDirectXContext.myQuadShader;
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

void CompileShader(const char* someShaderData, const char* anEntryPoint, const char* aVersion, ID3D10Blob*& aBufferOut)
{
    ID3DBlob* errorBlob = NULL;
   
    HRESULT result = D3DCompile(
        someShaderData,
        strlen(someShaderData) * sizeof(char),
        NULL,
        NULL,
        NULL,
        anEntryPoint,
        aVersion,
        0,
        0,
        &aBufferOut,
        &errorBlob);
    
    free((void*)someShaderData);
    if(result != S_OK)
    {
        const char* errorMsg = NULL;
        if(errorBlob)
            errorMsg = (const char*)errorBlob->GetBufferPointer();
        
        ASSERT(false);
    }
    
    if(errorBlob)
        errorBlob->Release();
}

unsigned int gfx_CreateShader(DX_shader& aShader, bool aSprite, const char* aVertexName, const char* aPixelName)
{
    char fullPath[100];
    int numChars = snprintf(fullPath, 100, "data/shaders/directx/%s", aVertexName);
    const char* vertexData = DE_ReadEntireFile(fullPath);
    
    numChars = snprintf(fullPath, 100, "data/shaders/directx/%s", aPixelName);
    const char* pixelData = DE_ReadEntireFile(fullPath);
    
    CompileShader(vertexData, "VS", "vs_5_0", aShader.myVertexShaderBuffer);
    CompileShader(pixelData, "PS", "ps_5_0", aShader.myPixelShaderBuffer);

    HRESULT result = ourDirectXContext.myDevice->CreateVertexShader(
        aShader.myVertexShaderBuffer->GetBufferPointer(),
        aShader.myVertexShaderBuffer->GetBufferSize(),
        NULL,
        &aShader.myVertexShader);
    ASSERT(result == S_OK);
    
    result = ourDirectXContext.myDevice->CreatePixelShader(
        aShader.myPixelShaderBuffer->GetBufferPointer(),
        aShader.myPixelShaderBuffer->GetBufferSize(),
        NULL,
        &aShader.myPixelShader);
    ASSERT(result == S_OK);
    
    
    if(aSprite)
    {
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);
    
        result = ourDirectXContext.myDevice->CreateInputLayout(
            layout,
            numElements,
            aShader.myVertexShaderBuffer->GetBufferPointer(),
            aShader.myVertexShaderBuffer->GetBufferSize(),
            &aShader.myInputLayout);
    }
    else 
    {
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        UINT numElements = ARRAYSIZE(layout);
        
        result = ourDirectXContext.myDevice->CreateInputLayout(
            layout,
            numElements,
            aShader.myVertexShaderBuffer->GetBufferPointer(),
            aShader.myVertexShaderBuffer->GetBufferSize(),
            &aShader.myInputLayout);
    }
        
    return 0;
}

unsigned int gfx_CreateHardcodedShader()
{
    //DX_shader& shader = ourDirectXContext.myQuadShader;
    //return gfx_CreateShader(shader, true, "quad.vx", "quad.px");
    
    DX_shader& shader = ourDirectXContext.myCubeShader;
    return gfx_CreateShader(shader, false, "cube.vx", "cube.px");
}

void gfx_BindShader(unsigned int aShaderID)
{
    //ourDirectXContext.myContext->VSSetShader(ourDirectXContext.myQuadShader.myVertexShader, 0, 0);
    //ourDirectXContext.myContext->PSSetShader(ourDirectXContext.myQuadShader.myPixelShader, 0, 0);
    //ourDirectXContext.myContext->IASetInputLayout(ourDirectXContext.myQuadShader.myInputLayout);
    ourDirectXContext.myContext->VSSetShader(ourDirectXContext.myCubeShader.myVertexShader, 0, 0);
    ourDirectXContext.myContext->PSSetShader(ourDirectXContext.myCubeShader.myPixelShader, 0, 0);
    ourDirectXContext.myContext->IASetInputLayout(ourDirectXContext.myCubeShader.myInputLayout);
    ourDirectXContext.myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void gfx_ShaderConstanti(unsigned int aShaderID, const char* aConstantName, int aValue)
{
}

void gfx_SetProjection(const Matrix& aProjection)
{
    ourDirectXContext.my3DConstants.myProjection = aProjection;
}

void gfx_SetView(const Matrix& aView)
{
    ourDirectXContext.my3DConstants.myView = aView;
}

void gfx_CommitConstantData()
{
    DX_constantBuffer& buffer = ourDirectXContext.myConstantBuffer;
    
    D3D11_MAPPED_SUBRESOURCE resource;
    HRESULT result = ourDirectXContext.myContext->Map(
        buffer.myBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &resource);
    ASSERT(result == S_OK);
    
    memcpy(resource.pData, &ourDirectXContext.my3DConstants, sizeof(ourDirectXContext.my3DConstants));
    
    ourDirectXContext.myContext->Unmap(
        buffer.myBuffer,
        0);
    
    ourDirectXContext.myContext->VSSetConstantBuffers(buffer.myBufferIndex, 1, &buffer.myBuffer);
}

unsigned int gfx_CreateTexture(int aWidth, int aHeight, bool aUseAlpha, void* someTextureData)
{
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = aWidth;
    textureDesc.Height = aHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA textureData = {};
    textureData.pSysMem = someTextureData;
    textureData.SysMemPitch = sizeof(unsigned char) * 4  * aWidth;
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
    ourDirectXContext.myContext->PSSetShaderResources(aTextureSlot, 1, &ourDirectXContext.myTexture.myShaderResource);
}


void gfx_DrawQuad()
{
    UINT stride = sizeof(DX_quadVertex);
    UINT offset = 0;
    
    ourDirectXContext.myContext->IASetVertexBuffers(
        0, 
        1, 
        &ourDirectXContext.myQuad.myVertexBuffer, 
        &stride, 
        &offset);
    
    ourDirectXContext.myContext->IASetIndexBuffer(
        ourDirectXContext.myQuad.myIndexBuffer, 
        DXGI_FORMAT_R32_UINT, 
        0);
    
    ourDirectXContext.myContext->DrawIndexed(6, 0, 0);
}

void gfx_DrawCube(const Matrix& aTransform)
{
    DX_renderobject& cube = ourDirectXContext.myCube;
    DX_constantBuffer& buffer = cube.myConstantBuffer;
    
    D3D11_MAPPED_SUBRESOURCE resource;
    HRESULT result = ourDirectXContext.myContext->Map(
        buffer.myBuffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &resource);
    ASSERT(result == S_OK);
    
    memcpy(resource.pData, &aTransform, sizeof(Matrix));
    
    ourDirectXContext.myContext->Unmap(
        buffer.myBuffer,
        0);
    
    ourDirectXContext.myContext->VSSetConstantBuffers(buffer.myBufferIndex, 1, &buffer.myBuffer);
    
    UINT stride = sizeof(DX_cubeVertex);
    UINT offset = 0;
    
    ourDirectXContext.myContext->IASetVertexBuffers(
        0, 
        1, 
        &cube.myVertexBuffer, 
        &stride, 
        &offset);
    
    ourDirectXContext.myContext->IASetIndexBuffer(
        cube.myIndexBuffer, 
        DXGI_FORMAT_R32_UINT, 
        0);
    
    ourDirectXContext.myContext->DrawIndexed(36, 0, 0);
}

void gfx_Clear()
{
    ourDirectXContext.myContext->ClearRenderTargetView(
        ourDirectXContext.myBackBuffer, 
        ourDirectXContext.myClearColor);
    
    ourDirectXContext.myContext->ClearDepthStencilView(
        ourDirectXContext.myDepthStencil,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.f,
        0);
}

void gfx_FinishFrame()
{
    ourDirectXContext.mySwapChain->Present(0, 0);
}
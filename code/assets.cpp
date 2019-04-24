#define MAX_IDENTIFIER_LENGHT 64

struct AssetInfo
{
    unsigned int myTextureID;
    Vector2f mySize;
    
    Vector4f myBoundingBox;
    float myAdvance;
};

struct Asset
{
    AssetInfo myInfo;
    char myIdentifier[MAX_IDENTIFIER_LENGHT];
};

struct FontAsset
{
    stbtt_fontinfo myFont;
    DE_File myFile;
    float myScaleFactor;
    
    GrowingArray<Asset> myBitmaps;
};

struct Assets
{
    FontAsset myDebugFont;
    GrowingArray<Asset> myBitmaps;
};

static Assets ourAssets;

void SetupDebugFont()
{
    FontAsset& asset = ourAssets.myDebugFont;
    asset.myFile = DE_ReadEntireFile("c:/Windows/Fonts/arial.ttf");
    const unsigned char* fontBuffer = (unsigned char*)asset.myFile.myContents;
    stbtt_InitFont(&asset.myFont, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0));
    
    float fontHeight = 256.f;
    asset.myScaleFactor = stbtt_ScaleForPixelHeight(&asset.myFont, fontHeight);
    
    ArrayAlloc(asset.myBitmaps, 16);
}

void SetupAssetStorage()
{
    ArrayAlloc(ourAssets.myBitmaps, 16);
    
    SetupDebugFont();
}

void FreeAssets()
{
    DE_FreeFile(ourAssets.myDebugFont.myFile);
}

AssetInfo GetBitmap(const char* aFilePath)
{
    ASSERT(strlen(aFilePath) < MAX_IDENTIFIER_LENGHT);
    
    for(int i = 0; i < ourAssets.myBitmaps.myCount; ++i)
    {
        const Asset& asset = ourAssets.myBitmaps[i];
        if(strcmp(asset.myIdentifier, aFilePath) == 0)
            return asset.myInfo;
    }
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(aFilePath, &width, &height, &nrChannels, 4);
    ASSERT(data != NULL);
    
    unsigned int texture = gfx_CreateTexture(width, height, RGBA, data);
    stbi_image_free(data);
    
    Asset& asset = ArrayAdd(ourAssets.myBitmaps);
    strcpy(asset.myIdentifier, aFilePath);
    asset.myInfo = {texture, {(float)width, (float)height}};
            
    return asset.myInfo;
}

AssetInfo GetCharBitmap(char aCharCode)
{
    for(int i = 0; i < ourAssets.myDebugFont.myBitmaps.myCount; ++i)
    {
        const Asset& asset = ourAssets.myDebugFont.myBitmaps[i];
        if(strlen(asset.myIdentifier) == 1 && asset.myIdentifier[0] == aCharCode)
            return asset.myInfo;
    }
    

    stbtt_fontinfo& font = ourAssets.myDebugFont.myFont;
    float scaleFactor = ourAssets.myDebugFont.myScaleFactor;
    
    int width;
    int height;
    int xOffset;
    int yOffset;
    unsigned char* bitmap = stbtt_GetCodepointBitmap(
        &font,
        0,
        scaleFactor,
        aCharCode,
        &width, 
        &height,
        &xOffset,
        &yOffset);

    int x0;
    int y0;
    int x1;
    int y1;
    
    stbtt_GetCodepointBox(&font, aCharCode, &x0, &y0, &x1, &y1);
    
    int  advance;
    stbtt_GetCodepointHMetrics(&font, aCharCode, &advance, NULL);
    
    unsigned int textureID = gfx_CreateTexture(width, height, SINGLE_CHANNEL, bitmap);
    stbtt_FreeBitmap(bitmap, NULL);
    
    Asset& asset = ArrayAdd(ourAssets.myDebugFont.myBitmaps);
    asset.myIdentifier[0] = aCharCode;
    asset.myIdentifier[1] = '\0';
    asset.myInfo.myTextureID = textureID;
    asset.myInfo.myBoundingBox.x = x0 * scaleFactor;
    asset.myInfo.myBoundingBox.y = y0 * scaleFactor;
    asset.myInfo.myBoundingBox.z = x1 * scaleFactor;
    asset.myInfo.myBoundingBox.w = y1 * scaleFactor;
    asset.myInfo.myAdvance = advance * scaleFactor;
    
    asset.myInfo.mySize.x = asset.myInfo.myBoundingBox.z - asset.myInfo.myBoundingBox.x;
    asset.myInfo.mySize.y = asset.myInfo.myBoundingBox.w - asset.myInfo.myBoundingBox.y;
    
    ArrayAdd(ourAssets.myDebugFont.myBitmaps, asset);
    
    return asset.myInfo;
}
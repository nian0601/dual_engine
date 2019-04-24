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

struct AssetStorage
{
    GrowingArray<Asset> myAssets;
};

static AssetStorage ourAssetStorage;

void SetupAssetStorage()
{
    ArrayAlloc(ourAssetStorage.myAssets, 16);
}

AssetInfo GetBitmap(const char* aFilePath)
{
    ASSERT(strlen(aFilePath) < MAX_IDENTIFIER_LENGHT);
    
    for(int i = 0; i < ourAssetStorage.myAssets.myCount; ++i)
    {
        const Asset& asset = ourAssetStorage.myAssets[i];
        if(strcmp(asset.myIdentifier, aFilePath) == 0)
            return asset.myInfo;
    }
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(aFilePath, &width, &height, &nrChannels, 4);
    ASSERT(data != NULL);
    
    unsigned int texture = gfx_CreateTexture(width, height, RGBA, data);
    stbi_image_free(data);
    
    Asset asset;
    strcpy(asset.myIdentifier, aFilePath);
    asset.myInfo = {texture, {(float)width, (float)height}};
    
    ArrayAdd(ourAssetStorage.myAssets, asset);
    return asset.myInfo;
}

AssetInfo GetCharBitmap(char aCharCode)
{
    for(int i = 0; i < ourAssetStorage.myAssets.myCount; ++i)
    {
        const Asset& asset = ourAssetStorage.myAssets[i];
        if(strlen(asset.myIdentifier) == 1 && asset.myIdentifier[0] == aCharCode)
            return asset.myInfo;
    }
    
    DE_File fontFile = DE_ReadEntireFile("c:/Windows/Fonts/arial.ttf");
    
    stbtt_fontinfo font;
    
    const unsigned char* fontBuffer = (unsigned char*)fontFile.myContents;
    stbtt_InitFont(&font, fontBuffer, stbtt_GetFontOffsetForIndex(fontBuffer, 0));
    
    int width;
    int height;
    int xOffset;
    int yOffset;
    float fontHeight = 256.f;
    float scaleFactor =  stbtt_ScaleForPixelHeight(&font, fontHeight);
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
    
    Asset asset;
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
    
    ArrayAdd(ourAssetStorage.myAssets, asset);
    DE_FreeFile(fontFile);
    
    return asset.myInfo;
}
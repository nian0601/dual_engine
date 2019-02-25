
int GetArrayIndex(int aX, int aZ)
{
    return aZ * GRIDSIZE + aX;
}

void ClearMap()
{
    Vector3f position;
    position.y = 0.f;
    Vector3f size = {CELLSIZE, CELLSIZE, CELLSIZE};
    for(int z = 0; z < GRIDSIZE; ++z)
    {
        for(int x = 0; x < GRIDSIZE; ++x)
        {
            position.x = x * CELLSIZE - ((GRIDSIZE * 0.5f) * CELLSIZE);
            position.z = z * CELLSIZE;
            
            int index = GetArrayIndex(x, z);
            Entity& tile = ourGameState.myMap.myTiles[index];
            PlaceEntity(tile, position, size);
            
            tile.myIsHighlighted = false;
            
            ourGameState.myMap.myTileTypes[index] = GRASS;
        }
    }
}

int GetNeighbourCount(TileType aNeighbourType, int aX, int aZ)
{
    int count = 0;
    for(int z = aZ - 1; z < aZ + 2; ++z)
    {
        if(z < 0 || z >= GRIDSIZE)
            continue;
        
        for(int x = aX - 1; x < aX + 2; ++x)
        {
            if(x < 0 || x >= GRIDSIZE)
                continue;
            
            if(x == aX && z == aZ)
                continue;
            
            if(ourGameState.myMap.myTileTypes[GetArrayIndex(x, z)] == aNeighbourType)
                ++count;
        }
    }
    
    return count;
}

void CelluarAutomataMutator(TileType aTypeToReplace, TileType aNewType, TileType aNeighourType, int aMinCount, int aMaxCount, float aChance)
{
    int memSize = sizeof(TileType) * GRIDSIZE * GRIDSIZE;
    TileType* newTiles = static_cast<TileType*>(malloc(memSize));
    memcpy(newTiles, &ourGameState.myMap.myTileTypes, memSize);
                                                
    for(int z = 0; z < GRIDSIZE; ++z)
    {
        for(int x = 0; x < GRIDSIZE; ++x)
        {
            int index = GetArrayIndex(x, z);
            if(newTiles[index] != aTypeToReplace)
                continue;
            
            int neighbourCount = GetNeighbourCount(aNeighourType, x, z);
            if(neighbourCount < aMinCount || neighbourCount > aMaxCount)
                continue;
            
            if(Random01() > aChance)
                continue;
            
            newTiles[index] = aNewType;
        }
    }
    
    memcpy(&ourGameState.myMap.myTileTypes, newTiles, memSize);
    free(newTiles);
}

void MultipleCelluar(TileType aTypeToReplace, TileType aNewType, TileType aNeighourType, int aMinCount, int aMaxCount, float aChance, int aCount)
{
    for(int i = 0; i < aCount; ++i)
        CelluarAutomataMutator(aTypeToReplace, aNewType, aNeighourType, aMinCount, aMaxCount, aChance);
}

void RandomMutator(TileType aNewType, float aChance)
{
    for(int i = 0; i < GRIDSIZE*GRIDSIZE; ++i)
    {
        if(Random01() > aChance)
            continue;
        
        ourGameState.myMap.myTileTypes[i] = aNewType;
    }
}

void RandomPlacementMutator(TileType aNewType, int aCount)
{
    for(int i = 0; i < aCount; ++i)
    {
        int index = RandomInt(1, GRIDSIZE*GRIDSIZE);
        --index;
        ourGameState.myMap.myTileTypes[index] = aNewType;
    }
}

void GenerateMap()
{
    ClearMap();
    RandomPlacementMutator(SPAWN_POSITION, 1);
    RandomMutator(SHALLOW_WATER, 0.01f);
    MultipleCelluar(GRASS, SHALLOW_WATER, SHALLOW_WATER, 1, 4, 0.4f, 4);
    CelluarAutomataMutator(GRASS, SHALLOW_WATER, SHALLOW_WATER, 4, 8, 1.f);
    CelluarAutomataMutator(SHALLOW_WATER, DEEP_WATER, SHALLOW_WATER, 8, 8, 1.f);
    
    CelluarAutomataMutator(GRASS, SAND, SHALLOW_WATER, 1, 8, 1.f);
    CelluarAutomataMutator(GRASS, DIRT, SAND, 4, 8, 1.f);
    MultipleCelluar(GRASS, DIRT, SAND, 2, 8, 0.2f, 4);
}

Entity* FindEntityOfTileType(TileType aTileType)
{
    for(int i = 0; i < GRIDSIZE*GRIDSIZE; ++i)
    {
        if(ourGameState.myMap.myTileTypes[i] == aTileType)
            return &ourGameState.myMap.myTiles[i];
    }
    
    return nullptr;
}

void RenderTile(int aIndex)
{
    Entity& tile = ourGameState.myMap.myTiles[aIndex];
    TileType type = ourGameState.myMap.myTileTypes[aIndex];
    
    Vector4f color;
    switch(type)
    {
        case GRASS: color = {0.03f, 0.67f, 0.1f, 1.f}; break;
        case SHALLOW_WATER: color = {0.09f, 0.12f, 0.89f, 1.f}; break;
        case DEEP_WATER: color = {0.10f, 0.12f, 0.49f, 1.f}; break;
        case DIRT:  color = {0.69f, 0.34f, 0.08f, 1.f}; break;
        case SAND:  color = {0.89f, 0.82f, 0.19f, 1.f}; break;
        case SPAWN_POSITION:  color = {0.1f, 0.04f, 0.07f, 1.f}; break;
    }
    
    if(tile.myIsHighlighted)
        color = {1.f, 1.f, 1.f, 1.f};
    
    RenderEntity(tile, color);
}


void RenderMap()
{
    for(int i = 0; i < GRIDSIZE*GRIDSIZE; ++i)
        RenderTile(i);
}
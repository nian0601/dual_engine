int GetArrayIndex(int aX, int aZ)
{
    return aZ * GRIDSIZE + aX;
}

void GetGridIndex(int aArrayIndex, int& aX, int& aZ)
{
    aX = aArrayIndex % GRIDSIZE;
    aZ = aArrayIndex / GRIDSIZE;
}

int GetArrayIndex(const Vector3f& aWorldPosition)
{
    int gridX = Round(aWorldPosition.x / CELLSIZE);
    int gridZ = Round(aWorldPosition.z / CELLSIZE);
    return GetArrayIndex(gridX, gridZ);
}

void ClearPathfindingNodes()
{
    for(int z = 0; z < GRIDSIZE; ++z)
    {
        for(int x = 0; x < GRIDSIZE; ++x)
        {
            int index = GetArrayIndex(x, z);
            PathNode& node = ourGameState.myMap.myPathNodes[index];
            
            node.myParent = nullptr;
            node.myCurrentCost = FLT_MAX;
            node.myHeuristic = 0;
            node.myTotalCost = 0;
            node.myState = PathNode::NOT_VISITED;
        }
    }   
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
            position.x = x * CELLSIZE;
            position.z = z * CELLSIZE;
            
            int index = GetArrayIndex(x, z);
            Entity& tile = ourGameState.myMap.myTiles[index];
            PlaceEntity(tile, position, size);
            
            tile.myIsHighlighted = false;
            tile.myMapIndex = index;
            
            ourGameState.myMap.myTileTypes[index] = GRASS;
            
            ourGameState.myMap.myPathNodes[index].myEntity = &tile;
        }
    }
    
    ClearPathfindingNodes();
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
    
    #if 0
    PathNode* pathNode = &ourGameState.myMap.myPathNodes[aIndex];
    switch(pathNode->myState)
    {
        case PathNode::IN_OPEN: color = {0.1f, 0.8f, 0.1f, 1.f}; break;
        case PathNode::IN_CLOSED: color = {1.f, 0.1f, 0.1f, 1.f}; break;
        //case PathNode::NOT_VISITED: color = {0.1f, 0.1f, 0.1f, 1.f}; break;
    }
    #endif
    
    RenderEntity(tile, color);
}


void RenderMap()
{
    for(int i = 0; i < GRIDSIZE*GRIDSIZE; ++i)
        RenderTile(i);
}


// MOVE TO OWN FILE AT SOME POINT
static int ourCostPerMove = 100;
float CalculateHeuristic(PathNode* aStart, PathNode* aGoal)
{
    int x1 = static_cast<int>(aStart->myEntity->myPosition.x);
    int x2 = static_cast<int>(aGoal->myEntity->myPosition.x);
    int z1 = static_cast<int>(aStart->myEntity->myPosition.z);
    int z2 = static_cast<int>(aGoal->myEntity->myPosition.z);
    
    return 10 * abs(x1 - x2) + abs(z1 - z2);
}

void UpdateNodeValues(PathNode* aParentNode, PathNode* aNode)
{   
    aNode->myParent = aParentNode;
    
    // Add IsStraight-check here to prefer straight checks, by adding a negative cost
    aNode->myCurrentCost = aParentNode->myCurrentCost + ourCostPerMove;
    aNode->myTotalCost = aNode->myCurrentCost + aNode->myHeuristic;
}

void EvaluateNeighbour(PointerHeap<PathNode*>& aHeap, PathNode* aCurrentNode, PathNode* aNeighbourNode, PathNode* aEndNode)
{
    if(aNeighbourNode->myState == PathNode::IN_CLOSED)
        return;
    
    TileType tileType = ourGameState.myMap.myTileTypes[aNeighbourNode->myEntity->myMapIndex];
    if(tileType == SHALLOW_WATER || tileType == DEEP_WATER)
        return;
    
    if(aNeighbourNode->myState == PathNode::IN_OPEN)
    {
        if(aNeighbourNode->myCurrentCost > aCurrentNode->myCurrentCost + ourCostPerMove)
        {
            UpdateNodeValues(aCurrentNode, aNeighbourNode);
            HeapHeapify(aHeap);
        }
    }
    else
    {
        aNeighbourNode->myHeuristic = CalculateHeuristic(aNeighbourNode, aEndNode);
        UpdateNodeValues(aCurrentNode, aNeighbourNode);
        
        aNeighbourNode->myState = PathNode::IN_OPEN;
        HeapAdd(aHeap, aNeighbourNode);
    }
}

bool Pathfind(Entity& aStart, Entity& aEnd, Path& aPathOut)
{
    ClearPathfindingNodes();
    ArrayClear(aPathOut.myPoints);
    
    PathNode* nodes = ourGameState.myMap.myPathNodes;
    
    PathNode* startNode = &nodes[aStart.myMapIndex];
    PathNode* endNode = &nodes[aEnd.myMapIndex];
    
    startNode->myCurrentCost = 0;
    startNode->myHeuristic = CalculateHeuristic(startNode, endNode);
    startNode->myTotalCost = startNode->myHeuristic + startNode->myCurrentCost;
    startNode->myState = PathNode::IN_OPEN;
    
    PointerHeap<PathNode*> heap = {};
    ArrayAlloc(heap.myData, 10);
    HeapAdd(heap, startNode);
    
    int x;
    int z;
    while(heap.myData.myCount > 0)
    {
        PathNode* currNode = HeapTake(heap);
        if(currNode->myEntity->myMapIndex == aEnd.myMapIndex)
        {
            const PathNode* node = currNode;
            while(node)
            {
                ArrayAdd(aPathOut.myPoints, node->myEntity);
                node = node->myParent;
            }
            
            aPathOut.myCurrentPoint = aPathOut.myPoints.myCount-1;
            return true;
        }
        
        GetGridIndex(currNode->myEntity->myMapIndex, x, z);
        
        if(x + 1 < GRIDSIZE)
        {
            PathNode* nextNode = &nodes[GetArrayIndex(x+1, z)];
            EvaluateNeighbour(heap, currNode, nextNode, endNode);
        }
        
        if(x - 1 >= 0)
        {
            PathNode* nextNode = &nodes[GetArrayIndex(x-1, z)];
            EvaluateNeighbour(heap, currNode, nextNode, endNode);
        }
        
        if(z + 1 < GRIDSIZE)
        {
            PathNode* nextNode = &nodes[GetArrayIndex(x, z+1)];
            EvaluateNeighbour(heap, currNode, nextNode, endNode);
        }
        
        if(z - 1 >= 0)
        {
            PathNode* nextNode = &nodes[GetArrayIndex(x, z-1)];
            EvaluateNeighbour(heap, currNode, nextNode, endNode);
        }
        
        currNode->myState = PathNode::IN_CLOSED;
    }
    
    return false;
}
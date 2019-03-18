void SetupGame()
{
    GenerateMap();    
    
    const Vector3f playerSize = {0.66f, 1.f, 0.66f};
    if(Entity* spawnPosition = FindEntityOfTileType(SPAWN_POSITION))
    {
        PlaceEntity(ourGameState.myPlayer, spawnPosition->myPosition, playerSize);
        PlaceEntityOnTop(ourGameState.myPlayer, *spawnPosition);
    }
    
    ArrayAlloc(ourGameState.myPlayerPath.myPoints, 10);
}

void UpdateCamera(float aDeltaTime, Matrix& aViewMatrix)
{
    float movement = 5.f;
    if(KeyDown(DEK_LSHIFT))
        movement = 15.f;
    
    movement *= aDeltaTime;
    
    if(KeyDown(DEK_A))
        TranslateRight(aViewMatrix, -movement);
    
    if(KeyDown(DEK_D))
        TranslateRight(aViewMatrix, movement);
    
    if(KeyDown(DEK_W))
        TranslateForward(aViewMatrix, movement);
    
    if(KeyDown(DEK_S))
        TranslateForward(aViewMatrix, -movement);
    
    if(KeyDown(DEK_Q))
        TranslateUp(aViewMatrix, -movement);
    
    if(KeyDown(DEK_E))
        TranslateUp(aViewMatrix, movement);
}

void HighlightEntityUnderMouse()
{
    float closestDistance = FLT_MAX;
    int closestIndex = -1;
    for(int i = 0; i < CELLCOUNT; ++i)
    {
        Entity& tile = ourGameState.myMap.myTiles[i];
        tile.myIsHighlighted = false;
        
        float distance = LineVSAABB(tile.myAABB, ourInput.myMouseRay, NULL);
        if(distance > -1.f && distance < closestDistance)
        {
            closestDistance = distance;
            closestIndex = i;
        }
    }
    
    ourGameState.myEntityUnderMouse = nullptr;
    if(closestIndex != -1)
    {
        ourGameState.myEntityUnderMouse = &ourGameState.myMap.myTiles[closestIndex];
        ourGameState.myEntityUnderMouse->myIsHighlighted = true;
    }
     
}

Vector3f GetCurrentPathTarget()
{
    Path& path = ourGameState.myPlayerPath;
    return GetPositionOnTopOfEntity(*path.myPoints[path.myCurrentPoint]);
}

bool UpdateEntityMovement(Entity& anEntity, float aDeltaTime)
{
    const float minDist = 0.15f * 0.15f;
    const float movementSpeed = 5.f;
    
    Vector3f direction = anEntity.myTargetPosition - anEntity.myPosition;
    float distance = Length2(direction);
    if(distance <= minDist)
    {
        anEntity.myPosition = anEntity.myTargetPosition;
        return true;
    }
    
    Normalize(direction);
    
    anEntity.myPosition += direction * movementSpeed * aDeltaTime;
    
    return false;
}

void FollowPath(float aDeltaTime)
{
    if(UpdateEntityMovement(ourGameState.myPlayer, aDeltaTime))
    {
        Path& path = ourGameState.myPlayerPath;
        if(path.myCurrentPoint <= 0)
            return;
        
        path.myCurrentPoint--;
        ourGameState.myPlayer.myTargetPosition = GetCurrentPathTarget();
    }
}

void UpdateGame(float aDeltaTime)
{
    HighlightEntityUnderMouse();
    
    if(MouseDownThisFrame(DEK_LEFTMOUSE) && ourGameState.myEntityUnderMouse)
    {
        int playerArrayIndex = GetArrayIndex(ourGameState.myPlayer.myPosition);
        Entity& entityUnderPlayer = ourGameState.myMap.myTiles[playerArrayIndex];
        if(Pathfind(entityUnderPlayer, *ourGameState.myEntityUnderMouse, ourGameState.myPlayerPath))
            ourGameState.myPlayer.myTargetPosition = GetCurrentPathTarget();   
    }
    
    FollowPath(aDeltaTime);
}

void RenderGame()
{
    RenderMap();
    RenderEntity(ourGameState.myPlayer, {0.79f, 0.89f, 0.83f, 1.f});
}
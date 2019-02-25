void SetupGame()
{
    GenerateMap();    
    
    const Vector3f playerSize = {0.66f, 1.f, 0.66f};
    if(Entity* spawnPosition = FindEntityOfTileType(SPAWN_POSITION))
    {
        PlaceEntity(ourGameState.myPlayer, spawnPosition->myPosition, playerSize);
        PlaceEntityOnTop(ourGameState.myPlayer, *spawnPosition);
    }
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

void UpdateEntityMovement(Entity& anEntity, float aDeltaTime)
{
    const float minDist = 0.15f * 0.15f;
    const float movementSpeed = 5.f;
    
    Vector3f direction = anEntity.myTargetPosition - anEntity.myPosition;
    float distance = Length2(direction);
    if(distance <= minDist)
    {
        anEntity.myPosition = anEntity.myTargetPosition;
        return;
    }
    
    Normalize(direction);
    
    anEntity.myPosition += direction * movementSpeed * aDeltaTime;
}

void UpdateGame(float aDeltaTime)
{
    HighlightEntityUnderMouse();
    
    if(MouseDownThisFrame(DEK_LEFTMOUSE) && ourGameState.myEntityUnderMouse)
        ourGameState.myPlayer.myTargetPosition = GetPositionOnTopOfEntity(*ourGameState.myEntityUnderMouse);
    
    UpdateEntityMovement(ourGameState.myPlayer, aDeltaTime);
}

void RenderGame()
{
    RenderMap();
    RenderEntity(ourGameState.myPlayer, {0.79f, 0.89f, 0.83f, 1.f});
}
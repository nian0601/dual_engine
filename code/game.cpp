void ToggleDebugCamera(Camera& aCamera)
{
    if(ourGameState.myUseDebugCamera)
    {
        Vector4f translation = GetTranslation(aCamera.myView);
        ourGameState.myPlayer.myPosition.x = translation.x;
        ourGameState.myPlayer.myPosition.y = translation.y;
        ourGameState.myPlayer.myPosition.z = translation.z;
        ourGameState.myUseDebugCamera = false;
    }
    else
    {
        ourGameState.myUseDebugCamera = true;
    }
}

Vector3f CalculateCameraPosition(float aDeltaTime, Camera& aCamera)
{    
    if(ourGameState.myUseDebugCamera)
    {
        float movement = 10.f;
        if(KeyDown(DEK_LSHIFT))
            movement = 50.f;
        
        movement *= aDeltaTime;
        
        Matrix& viewMatrix = aCamera.myView;
        if(KeyDown(DEK_A))
            TranslateRight(viewMatrix, -movement);
        
        if(KeyDown(DEK_D))
            TranslateRight(viewMatrix, movement);
        
        if(KeyDown(DEK_W))
            TranslateForward(viewMatrix, movement);
        
        if(KeyDown(DEK_S))
            TranslateForward(viewMatrix, -movement);
        
        if(KeyDown(DEK_Q))
            TranslateUp(viewMatrix, -movement);
        
        if(KeyDown(DEK_E))
            TranslateUp(viewMatrix, movement);
        
        Vector4f translation = GetTranslation(aCamera.myView);
        return {translation.x, translation.y, translation.z};
    }
    else
    {
        CameraEntity& cameraEntity = ourGameState.myCameraEntity;
        return cameraEntity.myParentEntity->myPosition + cameraEntity.myParentOffset;
    }
}

void UpdateCamera(float aDeltaTime, Camera& aCamera)
{
    float rotationSpeed = 3.14f * 0.1f * aDeltaTime;
    
    SetTranslation(aCamera.myView, {0.f, 0.f, 0.f});
    
    Matrix xRotation = RotationMatrixAxisAngle(aCamera.myView.myRows[0].myData, rotationSpeed * ourInput.myMouseDelta.y);
    Matrix yRotation = RotationMatrixY(rotationSpeed * ourInput.myMouseDelta.x);
    
    aCamera.myView = aCamera.myView * xRotation;
    aCamera.myView = aCamera.myView * yRotation;

    SetTranslation(aCamera.myView, CalculateCameraPosition(aDeltaTime, aCamera));
    
    aCamera.myInvertedView = InverseSimple(aCamera.myView);
    ourInput.myMouseRay.myStart = Unproject(
        ourInput.myMousePosition, 0.f, aCamera.myInvertedView, aCamera.myProjection, aCamera.myWindowSize);
    ourInput.myMouseRay.myEnd = Unproject(
        ourInput.myMousePosition, 1.f, aCamera.myInvertedView, aCamera.myProjection, aCamera.myWindowSize);
}

void UpdatePlayerVelocity(float aDeltaTime, Camera& aCamera)
{
    // Use the Cross-product to figure out where 'forward' is regardless
    // of X-rotation of the camera
    Vector3f right = aCamera.myView.myRows[0].myData;
    Vector3f up = {0.f, 1.f, 0.f};
    Vector3f forward = Cross(right, up);

    float moveSpeed = 10.f;
    right *= moveSpeed;
    forward *= moveSpeed;

    Entity& player = ourGameState.myPlayer;
    player.myVelocity.x = 0.f;
    player.myVelocity.z = 0.f;
    
    if(KeyDown(DEK_A))
        player.myVelocity -= right;
    if(KeyDown(DEK_D))
        player.myVelocity += right;
    
    if(KeyDown(DEK_W))
        player.myVelocity += forward;
    if(KeyDown(DEK_S))
        player.myVelocity -= forward;    
    
    if(KeyDownThisFrame(DEK_SPACE))
        player.myVelocity.y += 20.f * aDeltaTime;
    
    player.myVelocity.y -= 0.25f * aDeltaTime;
    
    player.myVelocity.x *= aDeltaTime;
    player.myVelocity.z *= aDeltaTime;
    
    if(MouseDownThisFrame(DEK_LEFTMOUSE))
    {
        DE_Ray clickRay;
        clickRay.myStart = player.myPosition;
        clickRay.myStart.y += 0.5f;
        
        clickRay.myEnd = clickRay.myStart;
        clickRay.myEnd += forward * 10.f;
        
        ModifyBlockUnderMouse(clickRay, InvalidBlockType);
    }
}

void UpdatePlayer(float aDeltaTime, Camera& aCamera)
{
    Entity& player = ourGameState.myPlayer;
    
    UpdatePlayerVelocity(aDeltaTime, aCamera);
    
    DE_Ray movementRay;
    movementRay.myStart = player.myPosition;
    movementRay.myThickness = 0.1f;
    
    GrowingArray<ChunkRaycastHit> hits;
    BlockRaycastHit closestBlockHit;
    ArrayAlloc(hits, 8);
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.y += player.myVelocity.y;
    if(DoRaycast(movementRay, true, hits))
    {
        closestBlockHit = GetClosestBlockHit(hits);
        
        player.myPosition.y = closestBlockHit.myHitPosition.y - player.myVelocity.y;
        player.myVelocity.y = 0.f;
    }
     
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.x += player.myVelocity.x * 2.f;
    
    ArrayClear(hits);
    if(DoRaycast(movementRay, true, hits))
    {
        closestBlockHit = GetClosestBlockHit(hits);
        
        player.myPosition.x = closestBlockHit.myHitPosition.x - player.myVelocity.x;
        player.myVelocity.x = 0.f;   
    }
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.z += player.myVelocity.z * 2.f;
    
    ArrayClear(hits);
    if(DoRaycast(movementRay, true, hits))
    {
        closestBlockHit = GetClosestBlockHit(hits);
        
        player.myPosition.z = closestBlockHit.myHitPosition.z - player.myVelocity.z;
        player.myVelocity.z = 0.f;
    }
    
    player.myPosition += player.myVelocity;
}
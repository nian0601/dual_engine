void UpdateCamera(float aDeltaTime, Camera& aCamera)
{
    Vector3f cameraPosition = ourGameState.myPlayerPosition;
    cameraPosition.y += 0.5f;
    
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
        cameraPosition = {translation.x, translation.y, translation.z};
    }

    float rotationSpeed = 3.14f * 0.1f * aDeltaTime;
    
    SetTranslation(aCamera.myView, {0.f, 0.f, 0.f});
    
    Matrix xRotation = RotationMatrixAxisAngle(aCamera.myView.myRows[0].myData, rotationSpeed * ourInput.myMouseDelta.y);
    Matrix yRotation = RotationMatrixY(rotationSpeed * ourInput.myMouseDelta.x);
    
    aCamera.myView = aCamera.myView * xRotation;
    aCamera.myView = aCamera.myView * yRotation;

    SetTranslation(aCamera.myView, cameraPosition);
    
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
    
    ourGameState.myPlayerVelocity.x = 0.f;
    ourGameState.myPlayerVelocity.z = 0.f;
    
    if(KeyDown(DEK_A))
        ourGameState.myPlayerVelocity -= right;
    if(KeyDown(DEK_D))
        ourGameState.myPlayerVelocity += right;
    
    if(KeyDown(DEK_W))
        ourGameState.myPlayerVelocity += forward;
    if(KeyDown(DEK_S))
        ourGameState.myPlayerVelocity -= forward;    
    
    if(KeyDownThisFrame(DEK_SPACE))
        ourGameState.myPlayerVelocity.y += 20.f * aDeltaTime;
    
    ourGameState.myPlayerVelocity.y -= 0.25f * aDeltaTime;
    
    ourGameState.myPlayerVelocity.x *= aDeltaTime;
    ourGameState.myPlayerVelocity.z *= aDeltaTime;
    
    if(MouseDownThisFrame(DEK_LEFTMOUSE))
    {
        DE_Ray clickRay;
        clickRay.myStart = ourGameState.myPlayerPosition;
        clickRay.myStart.y += 0.5f;
        
        clickRay.myEnd = clickRay.myStart;
        clickRay.myEnd += forward * 10.f;
        
        ModifyBlockUnderMouse(clickRay, InvalidBlockType);
    }
}

void UpdatePlayer(float aDeltaTime, Camera& aCamera)
{   
    UpdatePlayerVelocity(aDeltaTime, aCamera);
    
    DE_Ray movementRay;
    movementRay.myStart = ourGameState.myPlayerPosition;
    movementRay.myThickness = 0.1f;
    
    GrowingArray<ChunkRaycastHit> hits;
    BlockRaycastHit closestBlockHit;
    ArrayAlloc(hits, 8);
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.y += ourGameState.myPlayerVelocity.y;
    if(DoRaycast(movementRay, true, hits))
    {
        closestBlockHit = GetClosestBlockHit(hits);
        
        ourGameState.myPlayerPosition.y = closestBlockHit.myHitPosition.y - ourGameState.myPlayerVelocity.y;
        ourGameState.myPlayerVelocity.y = 0.f;
    }
     
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.x += ourGameState.myPlayerVelocity.x * 2.f;
    
    ArrayClear(hits);
    if(DoRaycast(movementRay, true, hits))
    {
        closestBlockHit = GetClosestBlockHit(hits);
        
        ourGameState.myPlayerPosition.x = closestBlockHit.myHitPosition.x - ourGameState.myPlayerVelocity.x;
        ourGameState.myPlayerVelocity.x = 0.f;   
    }
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.z += ourGameState.myPlayerVelocity.z * 2.f;
    
    ArrayClear(hits);
    if(DoRaycast(movementRay, true, hits))
    {
        closestBlockHit = GetClosestBlockHit(hits);
        
        ourGameState.myPlayerPosition.z = closestBlockHit.myHitPosition.z - ourGameState.myPlayerVelocity.z;
        ourGameState.myPlayerVelocity.z = 0.f;
    }
    
    ourGameState.myPlayerPosition += ourGameState.myPlayerVelocity;
}
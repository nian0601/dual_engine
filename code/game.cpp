void UpdateCamera(float aDeltaTime, gfx_camera& aCamera)
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
        
        if(MouseDown(DEK_RIGHTMOUSE))
        {
            if(Length(ourInput.myMouseDelta) > 0.f)
            {
                float rotationSpeed = 3.14f * 0.1f * aDeltaTime;
                
                
                Vector4f translation = GetTranslation(aCamera.myView);
                SetTranslation(aCamera.myView, {0.f, 0.f, 0.f});
                
                Matrix xRotation = RotationMatrixAxisAngle(aCamera.myView.myRows[0].myData, rotationSpeed * ourInput.myMouseDelta.y);
                Matrix yRotation = RotationMatrixY(rotationSpeed * ourInput.myMouseDelta.x);
                
                aCamera.myView = aCamera.myView * xRotation;
                aCamera.myView = aCamera.myView * yRotation;
                
                SetTranslation(aCamera.myView, {translation.x, translation.y, translation.z});
            }
        }
    }
    else
    {
        float rotationSpeed = 3.14f * 0.1f * aDeltaTime;
        
        SetTranslation(aCamera.myView, {0.f, 0.f, 0.f});
        Matrix yRotation = RotationMatrixY(rotationSpeed * ourInput.myMouseDelta.x);
        aCamera.myView = aCamera.myView * yRotation;
        
        Vector3f cameraPosition = ourGameState.myPlayerPosition;
        cameraPosition.y += 0.5f;
        SetTranslation(aCamera.myView, cameraPosition);
    }
    
    aCamera.myInvertedView = InverseSimple(aCamera.myView);
    ourInput.myMouseRay.myStart = Unproject(
        ourInput.myMousePosition, 0.f, aCamera.myInvertedView, aCamera.myProjection, aCamera.myWindowSize);
    ourInput.myMouseRay.myEnd = Unproject(
        ourInput.myMousePosition, 1.f, aCamera.myInvertedView, aCamera.myProjection, aCamera.myWindowSize);
}

void UpdatePlayer(float aDeltaTime, gfx_camera& aCamera)
{
    if(ourGameState.myUseDebugCamera)
        return;
    
    
    DE_Ray movementRay;
    movementRay.myStart = ourGameState.myPlayerPosition;
    
    GrowingArray<ChunkRaycastHit> hits;
    ArrayAlloc(hits, 8);
    
    float gravity = 9.82f * aDeltaTime;
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.y -= gravity * 2;
    if(DoRaycast(movementRay, true, hits))
        gravity = 0.f;
    
    
    Vector3f forward = aCamera.myView.myRows[2].myData;
    Vector3f right = aCamera.myView.myRows[0].myData;
    
    Vector3f moveDir = {};
    
    if(KeyDown(DEK_A))
        moveDir -= right;
    if(KeyDown(DEK_D))
        moveDir += right;
    
    if(KeyDown(DEK_W))
        moveDir += forward;
    if(KeyDown(DEK_S))
        moveDir -= forward;
    
    Normalize(moveDir);
    float moveSpeed = 10.f;
    
    moveDir *= moveSpeed;
    moveDir *= aDeltaTime;
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.x += moveDir.x;
    
    ArrayClear(hits);
    if(DoRaycast(movementRay, true, hits))
        moveDir.x = 0.f;
    
    movementRay.myEnd = movementRay.myStart;
    movementRay.myEnd.z += moveDir.z;
    
    ArrayClear(hits);
    if(DoRaycast(movementRay, true, hits))
        moveDir.z = 0.f;
    

    ourGameState.myPlayerPosition += moveDir;
    ourGameState.myPlayerPosition.y -= gravity;
    
    
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

void RenderPlayer()
{
    if(ourGameState.myUseDebugCamera)
        QueueCube(ourGameState.myPlayerPosition, {1.f, 1.f, 1.f, 1.f});
}
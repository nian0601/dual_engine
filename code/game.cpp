struct Entity
{
    Vector3f myPosition;
    Vector3f myVelocity;
    Vector3f mySize;
    Vector3f myHalfSize;
    Vector4f myColor;
};

#define MAX_NUM_CARS 32
#define MAX_NUM_ROAD_PARTS 3
#define MAX_NUM_FENCE_POSTS 64

struct GameState
{
    Entity myTank;
    Entity myCars[MAX_NUM_CARS];
    Entity myRoads[MAX_NUM_ROAD_PARTS];
    Entity myFencePosts[MAX_NUM_FENCE_POSTS];
    
    float myNextRoadZ;
    float myNextCarZ;
    int myNumPostsPerRoad;
    float myPostSpacing;
};
static GameState ourGameState;

void InitRoadPart(int aRoadIndex)
{
    Entity& road = ourGameState.myRoads[aRoadIndex];
    road.mySize.x = 8.f;
    road.mySize.y = 2.f;
    road.myHalfSize = road.mySize * 0.5f;
    road.myColor = {0.2f, 0.13f, 0.04f, 1.f};
    road.myPosition.z = ourGameState.myNextRoadZ;
    
    ourGameState.myNextRoadZ += road.mySize.z;
    
    int postBaseIndex = aRoadIndex * ourGameState.myNumPostsPerRoad;
    float roadStartZ = road.myPosition.z - road.mySize.z * 0.5f;
    for(int i = 0; i < ourGameState.myNumPostsPerRoad; i += 2)
    {
        Entity& leftPost = ourGameState.myFencePosts[postBaseIndex + i];
        Entity& rightPost = ourGameState.myFencePosts[postBaseIndex + i+1];
        
        leftPost.mySize = {0.1f, 1.f, 0.1f};
        leftPost.myColor = {0.6f, 0.25f, 0.09f, 1.f};
        leftPost.myPosition.x = -road.myHalfSize.x;
        leftPost.myPosition.y = 1.5f;
        leftPost.myPosition.z = roadStartZ + i * ourGameState.myPostSpacing;
        
        rightPost = leftPost;
        rightPost.myPosition.x *= -1.f;
    }
}

void InitRoad()
{
    const float roadPartSize = 100.f;
    ourGameState.myNumPostsPerRoad = MAX_NUM_FENCE_POSTS / MAX_NUM_ROAD_PARTS;
    ourGameState.myNextRoadZ = 50.f;
    ourGameState.myPostSpacing = roadPartSize / ourGameState.myNumPostsPerRoad;
    
    for(int i = 0; i < MAX_NUM_ROAD_PARTS; ++i)
    {
        ourGameState.myRoads[i].mySize.z = roadPartSize;
        InitRoadPart(i);
    }
}

void RespawnCar(Entity& aCar)
{
    const Entity& refRoad = ourGameState.myRoads[0];
    static const float laneWidth = refRoad.mySize.x / MAX_NUM_ROAD_PARTS;
    
    float x = refRoad.myPosition.x;
    x -= refRoad.myHalfSize.x;
    x += laneWidth * 0.5f;
    
    float positions[MAX_NUM_ROAD_PARTS] = {};
    for(int i = 0; i < MAX_NUM_ROAD_PARTS; ++i)
    {
        positions[i] = x;
        x += laneWidth;
    }
    
    aCar.myPosition.x = positions[rand() % MAX_NUM_ROAD_PARTS];
    aCar.myPosition.y = 1.5f;
    aCar.myPosition.z = ourGameState.myNextCarZ;
    
    ourGameState.myNextCarZ += 5.f;
}

void InitCars()
{
    ourGameState.myNextCarZ = 20.f;
    for(int i = 0; i < MAX_NUM_CARS; ++i)
    {
        Entity& car = ourGameState.myCars[i];
        car.mySize = {1.f, 0.7f, 1.f};
        car.myHalfSize = car.mySize * 0.5f;
        car.myColor = {0.83f, 0.13f, 0.21f, 1.f};
        car.myVelocity.z = -1.f;
        
        RespawnCar(car);
    }
}

void InitEntities()
{
    Entity& tank = ourGameState.myTank;
    tank.mySize = {1.f, 0.7f, 1.f};
    tank.myHalfSize = tank.mySize * 0.5f;
    tank.myColor = {0.5f, 1.f, 0.5f, 1.f};
    tank.myPosition.y = 1.7f;
    tank.myVelocity.z = 4.f;
    
    InitRoad();
    InitCars();
}

void UpdateTankVelocity()
{
    const float speed = 3.f;
    
    Vector3f& velocity = ourGameState.myTank.myVelocity;
    velocity.x = 0.f;
    velocity.z = 2.f;
    
    //A
    if(KeyDown(0x41))
        velocity.x += -speed;
    
    //D
    if(KeyDown(0x44))
        velocity.x += speed;
    
    //W
    if(KeyDown(0x57))
        velocity.z *= 4.f;
}

void ClampTankToRoad()
{
    Entity& tank = ourGameState.myTank;
    Entity& road = ourGameState.myRoads[0];
    
    float leftEdge = road.myPosition.x - road.myHalfSize.x + tank.mySize.x;
    float rightEdge = road.myPosition.x + road.myHalfSize.x - tank.mySize.x;
    
    if(tank.myPosition.x <= leftEdge && tank.myVelocity.x < 0.f)
    {
        tank.myPosition.x = leftEdge;
        tank.myVelocity.x = 0.f;
    }
    else if(tank.myPosition.x >= rightEdge && tank.myVelocity.x > 0.f)
    {
        tank.myPosition.x = rightEdge;
        tank.myVelocity.x = 0.f;
    }
}

bool IsBehindTank(const Entity& anEntity)
{
    float farZ = anEntity.myPosition.z + anEntity.mySize.z * 0.5f;
    return farZ < ourGameState.myTank.myPosition.z - 2.f;
}

void MoveRoadPartsIfNeeded()
{
    for(int i = 0; i < MAX_NUM_ROAD_PARTS; ++i)
    {
        Entity& road = ourGameState.myRoads[i];
        if(IsBehindTank(road))
            InitRoadPart(i);
    }
}

bool CheckCollision(const Entity& aA, const Entity& aB)
{
    const Vector3f minA = aA.myPosition - aA.myHalfSize;
    const Vector3f maxA = aA.myPosition + aA.myHalfSize;
    const Vector3f minB = aB.myPosition - aB.myHalfSize;
    const Vector3f maxB = aB.myPosition + aB.myHalfSize;
    
    if(minA.x > maxB.x) return false;
    if(minA.y > maxB.y) return false;
    if(minA.z > maxB.z) return false;
    
    if(maxA.x < minB.x) return false;
    if(maxA.y < minB.y) return false;
    if(maxA.z < minB.z) return false;
    
    return true;
}

void UpdateGame(float aDeltaTime)
{
    UpdateTankVelocity();
    ClampTankToRoad();
    
    Entity& tank = ourGameState.myTank;
    tank.myPosition += tank.myVelocity * aDeltaTime;
    
    for(int i = 0; i < MAX_NUM_CARS; ++i)
    {
        Entity& car = ourGameState.myCars[i];
        car.myPosition += car.myVelocity * aDeltaTime;
        
        if(IsBehindTank(car))
            RespawnCar(car);
        
        if(CheckCollision(tank, car))
            RespawnCar(car);
    }
    
    MoveRoadPartsIfNeeded();
}

void RenderEntity(Entity& anEntity)
{
    static Matrix dummyTransform;
    dummyTransform = IdentityMatrix();
    
    dummyTransform *= ScaleMatrix(anEntity.mySize);
    Translate(dummyTransform, anEntity.myPosition);
    
    gfx_DrawColoredCube(dummyTransform, anEntity.myColor);
}

void RenderGame()
{
    RenderEntity(ourGameState.myTank);
    
    for(int i = 0; i < MAX_NUM_ROAD_PARTS; ++i)
    {
        RenderEntity(ourGameState.myRoads[i]);
    }
    
    for(int i = 0; i < MAX_NUM_FENCE_POSTS; ++i)
    {
        RenderEntity(ourGameState.myFencePosts[i]);
    }
    
    for(int i = 0; i < MAX_NUM_CARS; ++i)
    {
        RenderEntity(ourGameState.myCars[i]);
    }
}

Vector3f GetCameraPosition()
{
    Entity& tank = ourGameState.myTank;
    
    static const Vector3f offset = {0.f, 3.f, -7.f};
    static const Vector3f mask = {0.f, 1.f, 1.f};
    return (tank.myPosition + offset) * mask;
}
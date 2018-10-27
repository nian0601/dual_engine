#include <gl/glu.h>

struct Vector3f
{
    float x;
    float y;
    float z;
};

struct Car
{
    Vector3f myPosition;
    float mySpeed;
};

struct GameState
{
    Car myCars[128];
    int myNumberOfCars;
    
    int myNumberOfRoadLanes;
    float myRoadLaneWidth;
    float myRoadLenght;
};

GameState ourGameState;

void renderCube(const Vector3f& aPosition, const Vector3f& aSize, const Vector3f& aRotation)
{    
    // Start Transformations for the Cube
    glPushMatrix();
    
    // We move before we rotate, so we dont get weird big rotation
    glTranslatef(aPosition.x, aPosition.y, aPosition.z);

    glScalef(aSize.x, aSize.y, aSize.z);
    
    glRotatef(aRotation.x, 1, 0, 0);
    glRotatef(aRotation.y, 0, 1, 0);
    glRotatef(aRotation.z, 0, 0, 1);
    
    float halfWidth = 0.5f;
    float halfHeight = 0.5f;
    float halfDepth = 0.5f;
    
    
    glBegin(GL_QUADS);
    
    // Top
    glColor3f(0.0f, 1.0f,0.0f);
    glVertex3f(-halfWidth, halfHeight, -halfDepth);
	glVertex3f(halfWidth, halfHeight, -halfDepth);
	glVertex3f(halfWidth, halfHeight, halfDepth);
	glVertex3f(-halfWidth, halfHeight, halfDepth);

	// Bottom
    glColor3f(0.0f, 0.7f,0.2f);
	glVertex3f(-halfWidth, -halfHeight, -halfDepth);
	glVertex3f(halfWidth, -halfHeight, -halfDepth);
    glVertex3f(halfWidth, -halfHeight, halfDepth);
    glVertex3f(-halfWidth, -halfHeight, halfDepth);

	// Left
    glColor3f(1.0f, 0.0f,0.0f);
	glVertex3f(-halfWidth, -halfHeight, halfDepth);
	glVertex3f(-halfWidth, -halfHeight, -halfDepth);
	glVertex3f(-halfWidth, halfHeight, -halfDepth);
	glVertex3f(-halfWidth, halfHeight, halfDepth);

	// Right
    glColor3f(0.7f, 0.2f,0.0f);
	glVertex3f(halfWidth, -halfHeight, halfDepth);
    glVertex3f(halfWidth, -halfHeight, -halfDepth);
	glVertex3f(halfWidth, halfHeight, -halfDepth);
	glVertex3f(halfWidth, halfHeight, halfDepth);

	// Front
    glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-halfWidth, -halfHeight, -halfDepth);
	glVertex3f(halfWidth, -halfHeight, -halfDepth);
	glVertex3f(halfWidth, halfHeight, -halfDepth);
	glVertex3f(-halfWidth, halfHeight, -halfDepth);

	// Back
    glColor3f(0.2f, 0.0f, 0.7f);
	glVertex3f(-halfWidth, -halfHeight, halfDepth);
	glVertex3f(halfWidth, -halfHeight, halfDepth);
	glVertex3f(halfWidth, halfHeight, halfDepth);
	glVertex3f(-halfWidth, halfHeight, halfDepth);
    
    glEnd();
    
    glPopMatrix();
}

void renderLine(const Vector3f& aStart, const Vector3f& aEnd)
{
    glBegin(GL_LINES);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(aStart.x, aStart.y, aStart.z);
    glVertex3f(aEnd.x, aEnd.y, aEnd.z);
    
    glEnd();
}


void renderRoad()
{
     // Draw the main road
    Vector3f roadPosition = {};
    roadPosition.x = 0.f;
    roadPosition.y = 0.f;
    roadPosition.z = ourGameState.myRoadLenght * 0.5f;
    
    Vector3f roadSize = {};
    roadSize.x = ourGameState.myRoadLaneWidth * ourGameState.myNumberOfRoadLanes;
    roadSize.y = 1.f;
    roadSize.z = ourGameState.myRoadLenght;
    
    Vector3f roadRotation = {};
    
    renderCube(roadPosition, roadSize, roadRotation);
    
    // Draw the lines on the road
    Vector3f lineStart = {};
    lineStart.x = roadSize.x * 0.5;
    lineStart.y = 0.6f;
    lineStart.z = 0.f;
   
    Vector3f lineEnd = lineStart;
    lineEnd.z = ourGameState.myRoadLenght;
    
    renderLine(lineStart, lineEnd);
   
    for(int i = 0; i < ourGameState.myNumberOfRoadLanes; ++i)
    {
        lineStart.x -= ourGameState.myRoadLaneWidth;
        lineEnd.x = lineStart.x;
        renderLine(lineStart, lineEnd);  
    }
}

void renderCar(const Car& aCar)
{
    static Vector3f carSize = {ourGameState.myRoadLaneWidth * 0.7f, 2.f, 3.f};
    static Vector3f carRotation = {0.f, 0.f, -90.f};
    
    renderCube(aCar.myPosition, carSize, carRotation);
}

void createCar(const Vector3f& aPosition, float aSpeed)
{
    ourGameState.myCars[ourGameState.myNumberOfCars].myPosition = aPosition;
    ourGameState.myCars[ourGameState.myNumberOfCars].mySpeed = aSpeed;
    
    ourGameState.myNumberOfCars++;
}

void corbetInit()
{
    ourGameState.myNumberOfRoadLanes = 5;
    ourGameState.myRoadLaneWidth = 3.f;
    ourGameState.myRoadLenght = 100.f;
    
    const float roadWidth = ourGameState.myNumberOfRoadLanes * ourGameState.myRoadLaneWidth;
    
    const float carSpeed = 5.f; 
    const float carZSpacing = 10.f;
    
    
    Vector3f carPosition = {0.f, 1.f, 0.f};
    ourGameState.myNumberOfCars = 0;
    while(ourGameState.myNumberOfCars < 128)
    {
        // Randomly choose which Lane to spawn in
        int placement = rand() % ourGameState.myNumberOfRoadLanes;
        
        // Move all the way to the right
        float xPos = roadWidth * 0.5f;
        
        // Make sure we're centered in the lane
        xPos -= ourGameState.myRoadLaneWidth * 0.5f;
        
        // Select the correct lane
        xPos -= ourGameState.myRoadLaneWidth * placement;
        
        carPosition.x = xPos;
        carPosition.z += carZSpacing;
        createCar(carPosition, carSpeed);
    }
}

void corbetSetWindowSize(int aWindowWidth, int aWindowHeight)
{
    glViewport(0, 0, aWindowWidth, aWindowHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aWindowWidth/(float)aWindowHeight, 0.1, 100.0);
}

void corbetUpdate()
{
    // This should be replaced with a real deltatime
    // The program runs at like 45938fps on my machine,
    // so normal 1.f/60f to "fake" a delta-time doesnt work really
    float deltaTime = 1.f/2400.f;
    
    for(int i = 0; i < ourGameState.myNumberOfCars; ++i)
    {
        Car& car = ourGameState.myCars[i];
        car.myPosition.z -= car.mySpeed * deltaTime;
    }
}

void corbetRender()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(0.f, 6.f, -4.f,
              0.f, 0.f, 10.f,
              0.f, 1.f, 0.f);
    
   
    glEnable( GL_DEPTH_TEST );
    
    renderRoad();
    
    // Draw all the Cars
    for(int i = 0; i < ourGameState.myNumberOfCars; ++i)
        renderCar(ourGameState.myCars[i]);
    
    
    glDisable( GL_DEPTH_TEST );
    
    // Push all the commands to the GPU
    glFinish();
}
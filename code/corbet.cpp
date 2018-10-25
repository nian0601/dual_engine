#include <gl/glu.h>

struct Vector3f
{
    float x;
    float y;
    float z;
};

struct GameState
{
    Vector3f myCubePosition;
    Vector3f myCubePosition1;
    Vector3f myCubePosition2;
    Vector3f myCubeRotation;
    Vector3f myCubeSize;
};

GameState ourGameState;

void renderCube(const Vector3f& aPosition, const Vector3f& aSize, const Vector3f& aRotation)
{
    
    // Start Transformations for the Cube
    glPushMatrix();
    
    // We move before we rotate, so we dont get weird big rotation
    glTranslatef(aPosition.x, aPosition.y, aPosition.z);
    
    glRotatef(aRotation.x, 1, 0, 0);
    glRotatef(aRotation.y, 0, 1, 0);
    glRotatef(aRotation.z, 0, 0, 1);

    glScalef(aSize.x, aSize.y, aSize.z);
    
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

void corbetInit()
{
    ourGameState.myCubePosition.x = 0.f;
    ourGameState.myCubePosition.y = 0.f;
    ourGameState.myCubePosition.z = 2.f;
    
    ourGameState.myCubePosition1.x = -2.f;
    ourGameState.myCubePosition1.y = 0.f;
    ourGameState.myCubePosition1.z = 2.f;
    
    ourGameState.myCubePosition2.x = 2.f;
    ourGameState.myCubePosition2.y = 0.f;
    ourGameState.myCubePosition2.z = 2.f;
    
    ourGameState.myCubeRotation.x = 0.f;
    ourGameState.myCubeRotation.y = 30.f;
    ourGameState.myCubeRotation.z = 0.f;
    
    ourGameState.myCubeSize.x = 1.f;
    ourGameState.myCubeSize.y = 2.f;
    ourGameState.myCubeSize.z = 1.f;
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
    
    float degreesPerSecond = 10.f;
    ourGameState.myCubeRotation.y += degreesPerSecond * deltaTime;
    
    float metersPerSecond = 1.f;
    float movementSpeed = metersPerSecond * deltaTime;
    ourGameState.myCubePosition.z += movementSpeed;
}

void corbetRender()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      
    
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // This sets the position and direction of the camera
    // Place the camera at: 0, 0, 0
    // Point the camera towards: 0, 0, 10 (so make it point forward)
    // and we use Y as our up-axis
    gluLookAt(0.f, 0.f, -1.f,
              0.f, 0.f, 1.f,
              0.f, 1.f, 0.f);
    
    
    glEnable( GL_DEPTH_TEST );
    
    renderCube(ourGameState.myCubePosition, ourGameState.myCubeSize, ourGameState.myCubeRotation);
    renderCube(ourGameState.myCubePosition1, ourGameState.myCubeSize, ourGameState.myCubeRotation);
    renderCube(ourGameState.myCubePosition2, ourGameState.myCubeSize, ourGameState.myCubeRotation);

    glDisable( GL_DEPTH_TEST );
    
    // Push all the commands to the GPU
    glFinish();
}
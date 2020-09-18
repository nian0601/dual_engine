// Look here for KeyCodes:
// https://www.glfw.org/docs/3.3.2/group__keys.html

#define DEK_0 48
#define DEK_1 49
#define DEK_2 50
#define DEK_3 51
#define DEK_4 52
#define DEK_5 53
#define DEK_6 54
#define DEK_7 55
#define DEK_8 56
#define DEK_9 57


#define DEK_Q 81
#define DEK_W 87
#define DEK_E 69
#define DEK_A 65
#define DEK_S 83
#define DEK_D 68

#define DEK_ESCAPE 256
#define DEK_LSHIFT 340

#define DEK_RIGHT 262
#define DEK_LEFT 263
#define DEK_DOWN 264
#define DEK_UP 265

#define DEK_LEFTMOUSE 0
#define DEK_RIGHTMOUSE 1
#define DEK_MOUSECOUNT 2

struct DE_Input
{
    HWND myWindowHandle;
    
    bool myKeyState[512];
    bool myPrevKeyState[512];
    
    bool myMouseState[DEK_MOUSECOUNT];
    bool myPrevMouseState[DEK_MOUSECOUNT];
    Vector2f myMousePosition;
    Vector2f myMouseDelta;
    DE_Ray myMouseRay;
};

static DE_Input ourInput;
void OnGLFWKeyboardCallback(int aKey, int aAction)
{
    bool keyUp = aAction == GLFW_RELEASE;
    ourInput.myKeyState[aKey] = !keyUp;
}

void OnGLFWMouseButtonCallback(int aButton, int aAction)
{
    if(aButton == GLFW_MOUSE_BUTTON_LEFT)
    {
        ourInput.myMouseState[DEK_LEFTMOUSE] = aAction == GLFW_PRESS;
    }
    else if(aButton == GLFW_MOUSE_BUTTON_RIGHT)
    {
        ourInput.myMouseState[DEK_RIGHTMOUSE] = aAction == GLFW_PRESS;
    }
}

void UpdateInputState(GLFWwindow* aWindow)
{
    memcpy(ourInput.myPrevKeyState, ourInput.myKeyState, sizeof(bool) * 512);
    memcpy(ourInput.myPrevMouseState, ourInput.myMouseState, sizeof(bool) * DEK_MOUSECOUNT);
    
    Vector2f prevPosition = ourInput.myMousePosition;
    
    double mouseX, mouseY;
    glfwGetCursorPos(aWindow, &mouseX, &mouseY);
    
    ourInput.myMousePosition.x = float(mouseX);
    ourInput.myMousePosition.y = float(mouseY);
    
    ourInput.myMouseDelta = (ourInput.myMousePosition - prevPosition);
}

// Keyboard
bool KeyDownThisFrame(int aKeyCode)
{ 
    return !ourInput.myPrevKeyState[aKeyCode] && ourInput.myKeyState[aKeyCode];
}

bool KeyDown(int aKeyCode)
{
    return ourInput.myKeyState[aKeyCode];
}

bool KeyUp(int aKeyCode)
{
    return ourInput.myPrevKeyState[aKeyCode] && !ourInput.myKeyState[aKeyCode];
}

// Mouse
bool MouseDownThisFrame(UCHAR aMouseKey)
{ 
    return !ourInput.myPrevMouseState[aMouseKey] && ourInput.myMouseState[aMouseKey];
}

bool MouseDown(UCHAR aMouseKey)
{
    return ourInput.myMouseState[aMouseKey];
}

bool MouseUp(UCHAR aMouseKey)
{
    return ourInput.myPrevMouseState[aMouseKey] && !ourInput.myMouseState[aMouseKey];
}
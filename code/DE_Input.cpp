// Look here for KeyCodes:
// https://handmade.network/wiki/2823-keyboard_inputs_-_scancodes,_raw_input,_text_input,_key_names

#define MAX_INPUT_BUFFER_SIZE 48


#define DEK_1 0x02,
#define DEK_2 0x03,
#define DEK_3 0x04,
#define DEK_4 0x05,
#define DEK_5 0x06,
#define DEK_6 0x07,
#define DEK_7 0x08,
#define DEK_8 0x09,
#define DEK_9 0x0A,
#define DEK_0 0x0B,


#define DEK_Q 0x10
#define DEK_W 0x11
#define DEK_E 0x12
#define DEK_A 0x1E
#define DEK_S 0x1F
#define DEK_D 0x20

#define DEK_ESCAPE 0x01
#define DEK_LSHIFT 0x2A

#define DEK_LEFT 0xE04B
#define DEK_RIGHT 0xE04D
#define DEK_UP 0xE048
#define DEK_DOWN 0xE050

#define DEK_LEFTMOUSE 0
#define DEK_RIGHTMOUSE 1
#define DEK_MOUSECOUNT 2

struct DE_Input
{
    HWND myWindowHandle;
    
    BYTE myInputBuffer[MAX_INPUT_BUFFER_SIZE];
    
    bool myKeyState[256];
    bool myPrevKeyState[256];
    
    bool myMouseState[DEK_MOUSECOUNT];
    bool myPrevMouseState[DEK_MOUSECOUNT];
    Vector2f myMousePosition;
    DE_Ray myMouseRay;
};

static DE_Input ourInput;

void OnInputMessage(WPARAM aWParam, LPARAM aLParam)
{
    UINT bufferSize;
    GetRawInputData((HRAWINPUT)aLParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
    
    ASSERT(bufferSize <= MAX_INPUT_BUFFER_SIZE);
    
    GetRawInputData((HRAWINPUT)aLParam, RID_INPUT, (LPVOID)ourInput.myInputBuffer, &bufferSize, sizeof(RAWINPUTHEADER));
    
    RAWINPUT* raw = (RAWINPUT*)ourInput.myInputBuffer;
    if(raw->header.dwType == RIM_TYPEMOUSE)
    {
        POINT p;
        if (GetCursorPos(&p))
        {
            if (ScreenToClient(ourInput.myWindowHandle, &p))
            {
                ourInput.myMousePosition.x = p.x;
                ourInput.myMousePosition.y = p.y;
            }
        }
        
        /*
        RI_MOUSE_LEFT_BUTTON_DOWN
        RI_MOUSE_LEFT_BUTTON_UP
        RI_MOUSE_MIDDLE_BUTTON_DOWN
        RI_MOUSE_MIDDLE_BUTTON_UP
        RI_MOUSE_RIGHT_BUTTON_DOWN
        RI_MOUSE_RIGHT_BUTTON_UP
        RI_MOUSE_BUTTON_1_DOWN
        RI_MOUSE_BUTTON_1_UP
        */
        
        if(raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
            ourInput.myMouseState[DEK_LEFTMOUSE] = true;
        else if (raw->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP)
            ourInput.myMouseState[DEK_LEFTMOUSE] = false;
        
        if(raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
            ourInput.myMouseState[DEK_RIGHTMOUSE] = true;
        else if (raw->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP)
            ourInput.myMouseState[DEK_RIGHTMOUSE] = false;
        
    }
    else if(raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        USHORT keyCode = raw->data.keyboard.MakeCode;
        
        bool keyUp = raw->data.keyboard.Flags & RI_KEY_BREAK;
        ourInput.myKeyState[keyCode] = !keyUp;
    }
}

void RegisterInputDevices(HWND aHwnd)
{
    RAWINPUTDEVICE rid[2];
    
    //Keyboard
    rid[0].usUsagePage = 1;
    rid[0].usUsage = 6;
    rid[0].dwFlags = 0;
    rid[0].hwndTarget = NULL;
    
    //Mouse
    rid[1].usUsagePage = 1;
    rid[1].usUsage = 2;
    rid[1].dwFlags = 0;
    rid[1].hwndTarget = NULL;
    
    BOOL result = RegisterRawInputDevices(rid, 2, sizeof(RAWINPUTDEVICE));
    ASSERT(result != FALSE);
    
    POINT p;
    if (GetCursorPos(&p))
    {
        if (ScreenToClient(aHwnd, &p))
        {
            ourInput.myMousePosition.x = p.x;
            ourInput.myMousePosition.y = p.y;
        }
    }
    
    ourInput.myWindowHandle = aHwnd;
}

void UpdateInputState()
{
    memcpy(ourInput.myPrevKeyState, ourInput.myKeyState, sizeof(bool) * 256);
    memcpy(ourInput.myPrevMouseState, ourInput.myMouseState, sizeof(bool) * DEK_MOUSECOUNT);
}

// Keyboard
bool KeyDownThisFrame(UCHAR aKeyCode)
{ 
    return !ourInput.myPrevKeyState[aKeyCode] && ourInput.myKeyState[aKeyCode];
}

bool KeyDown(UCHAR aKeyCode)
{
    return ourInput.myKeyState[aKeyCode];
}

bool KeyUp(UCHAR aKeyCode)
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
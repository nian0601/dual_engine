#define MAX_INPUT_BUFFER_SIZE 48

struct DE_Input
{
    BYTE myInputBuffer[MAX_INPUT_BUFFER_SIZE];
    
    bool myKeyState[256];
    bool myPrevKeyState[256];
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
    }
    else if(raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        USHORT keyCode = raw->data.keyboard.VKey;
        
        bool keyUp = raw->data.keyboard.Flags & RI_KEY_BREAK;
        ourInput.myKeyState[keyCode] = !keyUp;
    }
}

void RegisterInputDevices()
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
}

void UpdateInputState()
{
    memcpy(ourInput.myPrevKeyState, ourInput.myKeyState, sizeof(bool) * 256);
}

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
    return ourInput.myPrevKeyState[aKeyCode] &&  !ourInput.myKeyState[aKeyCode];
}
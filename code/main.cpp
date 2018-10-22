#define GL_LITE_IMPLEMENTATION
#include "gl_lite.h"

#define ASSERT(aCondition){ if(!aCondition) { int* ___ptr = nullptr; *___ptr = 0;}}

#include <stdio.h>
#include <windows.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec3 vertexColor;\n"
"out vec2 texCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   vertexColor = aColor;\n"
"   texCoord = aTexCoord;\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 vertexColor;\n"
"in vec2 texCoord;\n"
"uniform sampler2D ourTexture1;\n"
"uniform sampler2D ourTexture2;\n"
"void main()\n"
"{\n"
"   FragColor = mix(texture(ourTexture1, texCoord), texture(ourTexture2, texCoord), 0.2);\n"
//"   FragColor = texture(ourTexture2, texCoord);\n"
//"   FragColor = vec4(vertexColor, 1.f); \n"
"}\n\0";

unsigned int ourShaderProgram;
unsigned int ourVertexArray;
unsigned int ourTexture1;
unsigned int ourTexture2;

void DrawOpenGL()
{
    glBegin(GL_TRIANGLES);
    
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2i(0,  1);
    
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2i(-1, -1);
    
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2i(1, -1);
    
    glEnd();
}

void VerifyShader(unsigned int aShaderID)
{
    int success = 0;
    glGetShaderiv(aShaderID, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(aShaderID, 512, NULL, infoLog);
        ASSERT(false);
    }
}

void VerifyShaderProgram(unsigned aShaderProgramID)
{
    int success = 0;
    glGetProgramiv(aShaderProgramID, GL_LINK_STATUS, &success);
    if(!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(aShaderProgramID, 512, NULL, infoLog);
        ASSERT(false);
    }
}

void CreateOpenGLModel()
{
    glGenVertexArrays(1, &ourVertexArray);
    glBindVertexArray(ourVertexArray);
    
    // Create our Vertices
    float size = 0.5f;
    float vertices[] = {
        // position         // colors            //UVs
         size,  size, 0.f,   1.f,  0.f,  0.f,    1.f, 1.0f,  // topright
         size, -size, 0.f,   0.f,  1.f,  0.f,    1.f, 0.f,   // bottomright
        -size, -size, 0.f,   0.f,  0.f,  1.f,    0.f, 0.f,   // bottomleft
        -size,  size, 0.f,   0.2f, 0.2f, 0.2f,   0.f, 1.f     // topleft
    };
    
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    // Create a VertexBuffer
    unsigned int vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Create a IndexBuffer
    unsigned int elementBufferObject;
    glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Compile our VertexShader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    VerifyShader(vertexShader);
    
    // Compile our FragmentShader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    VerifyShader(fragmentShader);
    
    // Link our Vertex and Fragment Shaders
    ourShaderProgram = glCreateProgram();
    glAttachShader(ourShaderProgram, vertexShader);
    glAttachShader(ourShaderProgram, fragmentShader);
    glLinkProgram(ourShaderProgram);
    VerifyShaderProgram(ourShaderProgram);

    // After we have linked the shaders, we dont need the objects anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);    
    

    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &ourTexture1);
    glBindTexture(GL_TEXTURE_2D, ourTexture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    ASSERT(data != NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    
    glGenTextures(1, &ourTexture2);
    glBindTexture(GL_TEXTURE_2D, ourTexture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
    ASSERT(data != NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
}

LRESULT CALLBACK WndProc(HWND aHwnd, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
    switch (aMessage)
    {
		case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(aHwnd, &ps);
            EndPaint(aHwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE:
        case WM_ACTIVATE:
		case WM_ENTERSIZEMOVE:
        case WM_EXITSIZEMOVE:
        {
            return 0;
        }
		//case WM_CHAR:			msg.myType = CE_WindowMessage::CHARACTER; break;
    }
    
   // return 0;
    return DefWindowProcA(aHwnd, aMessage, aWParam, aLParam);
}

HWND Win32CreateWindow(const char* aTitle, int aWindowWidth, int aWindowHeight)
{
    WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
    
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(NULL);
	wcex.hIcon = LoadIcon(wcex.hInstance, NULL);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = aTitle;
	wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);
    
    ASSERT(RegisterClassEx(&wcex) != FALSE);
    
    RECT rc = {0, 0, aWindowWidth, aWindowHeight};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    
    
    HWND windowHandle = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        aTitle,
        aTitle,
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        rc.right - rc.left,
        rc.bottom - rc.top,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    
    ASSERT(windowHandle != NULL);
    return windowHandle;
}

void Win32InitOpenGL(HWND aWindowHandle)
{
    HDC windowDC = GetDC(aWindowHandle);
    PIXELFORMATDESCRIPTOR desiredPixelFormat = {};
    desiredPixelFormat.nSize = sizeof(desiredPixelFormat);
    desiredPixelFormat.nVersion = 1;
    desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
    desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL|PFD_DRAW_TO_WINDOW;
    desiredPixelFormat.cColorBits = 32;
    desiredPixelFormat.cAlphaBits = 8;
    desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    int suggestedPixelFormatIndex = ChoosePixelFormat(windowDC, &desiredPixelFormat);
    
    PIXELFORMATDESCRIPTOR suggestedPixelFormat;
    DescribePixelFormat(windowDC, suggestedPixelFormatIndex, sizeof(suggestedPixelFormat), &suggestedPixelFormat);
    SetPixelFormat(windowDC, suggestedPixelFormatIndex, &suggestedPixelFormat);
    
    HGLRC openGLRC = wglCreateContext(windowDC);
    if(wglMakeCurrent(windowDC, openGLRC))
    {
        // Yay, we got a OpenGL-context!
    }
    else
    {
        ASSERT(false);
    }
    
    if(gl_lite_init())
    {
        // Yay, we managed to init GLAD
    }
    else
    {
        ASSERT(false);
    }
}

int main(int argc, char** argv)
{
    const char* windowTitle = "Dual Engine";
    
    const int windowWidth = 1280;
    const int windowHeight = 720;
    HWND windowHandle = Win32CreateWindow(windowTitle, windowWidth, windowHeight);
    Win32InitOpenGL(windowHandle);
    ShowWindow(windowHandle, 10);
    UpdateWindow(windowHandle);
    
    CreateOpenGLModel();
    glViewport(0, 0, windowWidth, windowHeight);
    
    MSG msg = {};
    bool isRunning = true;
    HDC deviceContext = GetDC(windowHandle);
 
   
    glClearColor(1.f, 0.f, 0.f, 1.f);
    glUseProgram(ourShaderProgram);
    glUniform1i(glGetUniformLocation(ourShaderProgram, "ourTexture1"), 0);
    glUniform1i(glGetUniformLocation(ourShaderProgram, "ourTexture2"), 1);
    
    while(isRunning)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            UINT msgCode = msg.message;
            if(msgCode == WM_QUIT || msgCode == WM_DESTROY)
                isRunning = false;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
 
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        glActiveTexture(GL_TEXTURE0+0);
        glBindTexture(GL_TEXTURE_2D, ourTexture1);
        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, ourTexture2);
        glBindVertexArray(ourVertexArray);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glFinish();
    }
    
    return 0;
}
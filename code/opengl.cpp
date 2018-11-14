#define GL_LITE_IMPLEMENTATION
#include "gl_lite.h"


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
"   FragColor = texture(ourTexture1, texCoord);\n"
"}\n\0";


struct opengl_renderobject
{  
    unsigned int myVertexArrayObject;
    unsigned int myVertexBufferObject;
    unsigned int myElementBufferObject;
};

struct opengl_context
{
    opengl_renderobject myQuad;
};

static opengl_context ourOpenGLContext;

void openglCreateQuad()
{
    opengl_renderobject quad = {};
    
    glGenVertexArrays(1, &quad.myVertexArrayObject);
    glBindVertexArray(quad.myVertexArrayObject);
    
    // Create our Vertices
    float size = 0.5f;
    float vertices[] = {
        // position         // colors            //UVs
         size,  size, 0.f,   1.f,  0.f,  0.f,    1.f, 0.0f,  // topright
         size, -size, 0.f,   0.f,  1.f,  0.f,    1.f, 1.f,   // bottomright
        -size, -size, 0.f,   0.f,  0.f,  1.f,    0.f, 1.f,   // bottomleft
        -size,  size, 0.f,   0.2f, 0.2f, 0.2f,   0.f, 0.f     // topleft
    };
    
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    
    // Create a VertexBuffer
    glGenBuffers(1, &quad.myVertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, quad.myVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Create a IndexBuffer
    glGenBuffers(1, &quad.myElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad.myElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
 
    
    // Setup VertexAttributes (input-layout)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);    
    
    
    ourOpenGLContext.myQuad = quad;
}


void openglVerifyShader(unsigned int aShaderID)
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

void openglVerifyShaderProgram(unsigned int aShaderProgramID)
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



// gfx_interface-implementation
void gfx_Init(HWND aWindowHandle, int aWindowHeight, int aWindowWidth)
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
    bool result = wglMakeCurrent(windowDC, openGLRC);
    ASSERT(result == true);
    
    result = gl_lite_init();
    ASSERT(result == true);
    
    openglCreateQuad();
}

void gfx_Shutdown()
{
}

void gfx_Viewport(int aX, int aY, int aWidth, int aHeight)
{
    glViewport(aX, aY, aWidth, aHeight);
}

void gfx_ClearColor(float aR, float aG, float aB)
{
    glClearColor(aR, aG, aB, 1.f);
}

unsigned int gfx_CreateShader(const char* aVertexShaderData, const char* aFragmentShaderData)
{
    // Compile our VertexShader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &aVertexShaderData, NULL);
    glCompileShader(vertexShader);
    openglVerifyShader(vertexShader);
    
    // Compile our FragmentShader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &aFragmentShaderData, NULL);
    glCompileShader(fragmentShader);
    openglVerifyShader(fragmentShader);
    
    // Link our Vertex and Fragment Shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    openglVerifyShaderProgram(shaderProgram);

    // After we have linked the shaders, we dont need the objects anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

unsigned int gfx_CreateHardcodedShader()
{
    return gfx_CreateShader(vertexShaderSource, fragmentShaderSource);
}

void gfx_BindShader(unsigned int aShaderID)
{
    glUseProgram(aShaderID);
}

void gfx_ShaderConstanti(unsigned int aShaderID, const char* aConstantName, int aValue)
{
    glUniform1i(glGetUniformLocation(aShaderID, aConstantName), aValue);
}

unsigned int gfx_CreateTexture(int aWidth, int aHeight, bool aUseAlpha, void* someTextureData)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, aWidth, aHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, someTextureData);
    
    return texture;
}

void gfx_BindTexture(unsigned int aTextureID, unsigned int aTextureSlot)
{
    glActiveTexture(GL_TEXTURE0 + aTextureSlot);
    glBindTexture(GL_TEXTURE_2D, aTextureID);
}

void gfx_DrawQuad()
{
    glBindVertexArray(ourOpenGLContext.myQuad.myVertexArrayObject);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void gfx_Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void gfx_FinishFrame()
{
    glFinish();
}
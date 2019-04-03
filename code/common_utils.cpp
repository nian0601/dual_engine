#include <stdio.h>
#include <stdlib.h>

struct DE_File
{
    const char* myContents;
    long myFileSize;
};

DE_File DE_ReadEntireFile(const char* aFilePath)
{
    FILE* file = fopen(aFilePath, "rb");
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* string = (char*)malloc(fileSize + 1);
    fread(string, fileSize, 1, file);
    fclose(file);
    
    string[fileSize] = 0;
    
    DE_File result;
    result.myContents = string;
    result.myFileSize = fileSize;
    return result;
}

void DE_FreeFile(DE_File& aFile)
{
    free((void*)aFile.myContents);
    aFile = {};
}

template <typename T>
void Swap(T& aA, T& aB)
{
    T temp = aA;
    aA = aB;
    aB = temp;
}
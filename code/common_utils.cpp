#include <stdio.h>
#include <stdlib.h>

const char* DE_ReadEntireFile(const char* aFilePath)
{
    FILE* file = fopen(aFilePath, "rb");
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* string = (char*)malloc(fileSize + 1);
    fread(string, fileSize, 1, file);
    fclose(file);
    
    string[fileSize] = 0;
    return string;
}

template <typename T>
void Swap(T& aA, T& aB)
{
    T temp = aA;
    aA = aB;
    aB = temp;
}
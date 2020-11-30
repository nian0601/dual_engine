#include <stdio.h>
#include <stdlib.h>
#include <shared_mutex>

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

template <typename T>
T Max(T a, T b)
{
    return a > b ? a : b;
}

template <typename T>
T Min(T a, T b)
{
    return a < b ? a : b;
}

///////////////////////////////////////////////////////////
// Threading-stuff, move to its own file?

struct Mutex
{
    void ReadLock()
    {
        myMutex.lock_shared();
    }
    void ReadUnlock()
    {
        myMutex.unlock_shared();
    }
    
    void ReadWriteLock()
    {
        myMutex.lock();
    }
    
    void ReadWriteUnlock()
    {
        myMutex.unlock();
    }
    
    std::shared_mutex myMutex;
};

struct ReadLock
{
    ReadLock(Mutex& aMutex)
        : myMutex(aMutex)
    {
        myMutex.ReadLock();
    }
    
    ~ReadLock()
    {
        myMutex.ReadUnlock();
    }
    
    Mutex& myMutex;
};

struct ReadWriteLock
{
    ReadWriteLock(Mutex& aMutex)
        : myMutex(aMutex)
    {
        myMutex.ReadWriteLock();
    }
    
    ~ReadWriteLock()
    {
        myMutex.ReadWriteUnlock();
    }
    
    Mutex& myMutex;
};
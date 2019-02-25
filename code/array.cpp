template <typename T>
struct GrowingArray
{
    int myCount;
    int myMaxCount;
    T* myData;
};

template <typename T>
void ArrayAlloc(GrowingArray<T>& anArray, int aCount)
{
    anArray.myMaxCount += aCount;
    
    T* oldData = anArray.myData;
    anArray.myData = static_cast<T*>(malloc(sizeof(T) * anArray.myMaxCount));
    memset(anArray.myData, 0, sizeof(T) * anArray.myMaxCount);
    memcpy(anArray.myData, oldData, sizeof(T) * anArray.myCount);
    free(oldData);
}

template <typename T>
void ArrayAdd(GrowingArray<T>& anArray, const T& anItem)
{
    if(anArray.myCount >= anArray.myMaxCount - 1)
        ArrayAlloc(anArray, anArray.myMaxCount * 2);
    
    anArray.myData[anArray.myCount] = anItem;
    anArray.myCount++;
}

template <typename T>
void ArrayClear(GrowingArray<T>& anArray)
{
    anArray.myCount = 0;
}

template <typename T>
void ArrayRemoveCyclic(GrowingArray<T>& anArray, int anIndex)
{
    ASSERT(anArray.myCount >= anIndex);
    ASSERT(anIndex >= 0);
    
    anArray.myCount--;
    anArray.myData[anIndex] = anArray.myData[anArray.myCount];
}
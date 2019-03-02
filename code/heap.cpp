template <typename T>
struct Heap
{
    GrowingArray<T> myData;
};

template <typename T>
void HeapAdd(Heap<T>& aHeap, const T& anItem)
{
    GrowingArray<T>& data = aHeap.myData;
    
    ArrayAdd(data, anItem);
    int pos = data.myCount - 1;
    while(pos > 0 && HeapCompare(aHeap, pos, (pos-1)/2))
    {
        Swap(data[pos], data[(pos-1)/2]);
        pos = (pos-1) / 2;
    }
}

template <typename T>
T HeapTake(Heap<T>& aHeap)
{
    T result = {};
    GrowingArray<T>& data = aHeap.myData;
    if(data.myCount == 0)
        return result;
    
    result = data[0];
    ArrayRemoveCyclic(data, data.myCount-1);
    
    int pos = 0;
    int lastNonLeaf = data.myCount / 2 - 1;
    
    while(pos < lastNonLeaf &&
          (HeapCompare(aHeap, pos, pos * 2 + 1) || HeapCompare(aHeap, pos, pos * 2 + 2)))
    {
        if(HeapCompare(aHeap, pos * 2 + 1, pos * 2 + 2))
        {
            Swap(data[pos], data[pos * 2 + 1]);
            pos = pos * 2 + 1;
        }
        else
        {
            Swap(data[pos], data[pos * 2 + 2]);
            pos = pos * 2 + 2;
        }
    }
    
    if(pos * 2 + 1 < data.myCount && HeapCompare(aHeap, pos, pos * 2 + 1))
        Swap(data[pos], data[pos * 2 + 1]);
    if(pos * 2 + 2 < data.myCount && HeapCompare(aHeap, pos, pos * 2 + 2))
       Swap(data[pos], data[pos * 2 + 2]);
    
    return result;
}

template <typename T>
void HeapHeapify(Heap<T>& aHeap)
{
    int lastNonLeaf = aHeap.myData.myCount / 2 - 1;
    for(int i = lastNonLeaf; i >= 0; --i)
        HeapMoveDown(aHeap, i, aHeap.myData.myCount -1);
}

template <typename T>
void HeapMoveDown(Heap<T>& aHeap, int aFirst, int aLast)
{
    int first = aFirst;
    int largest = 2 * first + 1;
    
    while(largest <= aLast)
    {
        if (largest < aLast && HeapCompare(aHeap, largest, largest + 1))
            largest++;
        
        if(HeapCompare(aHeap, largest, first))
        {
            Swap(aHeap.myData[largest], aHeap.myData[first]);
            first = largest;
            largest = 2 * first + 1;
        }
        else
        {
            largest = aLast + 1;
        }
    }
}

template <typename T>
bool HeapCompare(Heap<T>& aHeap, int aFirst, int aSecond)
{
    return aHeap.myData[aFirst] < aHeap.myData[aSecond];
}

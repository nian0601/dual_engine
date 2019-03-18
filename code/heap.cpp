template <typename T>
struct NormalComparer
{
    bool operator()(const T a, const T b) { return a < b; }
};

template <typename T>
struct PointerComparer
{
    bool operator()(const T a, const T b) { return *a < *b; }
};

template <typename T, typename Comparer>
struct HeapBase
{
    GrowingArray<T> myData;
    Comparer myComparer;
};


template <typename T>
using Heap = HeapBase<T, NormalComparer<T>>;

template <typename T>
using PointerHeap = HeapBase<T, PointerComparer<T>>;


template <typename T, typename Comparer>
void HeapAdd(HeapBase<T, Comparer>& aHeap, const T& anItem)
{
    GrowingArray<T>& data = aHeap.myData;
    
    ArrayAdd(data, anItem);
    int pos = data.myCount - 1;
    while(pos > 0 && HeapCompare(aHeap, pos, (pos-1)/2))
    {
        Swap(data[(pos - 1) / 2], data[pos]);
        pos = (pos - 1) / 2;
    }
}

template <typename T, typename Comparer>
T HeapTake(HeapBase<T, Comparer>& aHeap)
{
    T result = {};
    GrowingArray<T>& data = aHeap.myData;
    result = data[0];
    ArrayRemoveCyclic(data, 0);
    
    int pos = 0;
    int lastNonLeaf = data.myCount / 2 - 1;
    
    while(pos < lastNonLeaf
          && (HeapCompare(aHeap, pos * 2 + 1, pos) || HeapCompare(aHeap, pos * 2 + 2, pos)))
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
    
    if(pos * 2 + 1 < data.myCount && HeapCompare(aHeap, pos * 2 + 1, pos))
        Swap(data[pos], data[pos * 2 + 1]);
    
    if(pos * 2 + 2 < data.myCount && HeapCompare(aHeap, pos * 2 + 2, pos))
        Swap(data[pos], data[pos * 2 + 2]);
    
    return result;
}

template <typename T, typename Comparer>
void HeapHeapify(HeapBase<T, Comparer>& aHeap)
{
    int lastNonLeaf = aHeap.myData.myCount / 2 - 1;
    for(int i = lastNonLeaf; i >= 0; --i)
        HeapMoveDown(aHeap, i, aHeap.myData.myCount - 1);
}

template <typename T, typename Comparer>
void HeapMoveDown(HeapBase<T, Comparer>& aHeap, int aFirst, int aLast)
{
    int first = aFirst;
    int largest = 2 * first + 1;
    
    while(largest <= aLast)
    {
        if (largest < aLast && HeapCompare(aHeap, largest + 1, largest))
            ++largest;
        
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

template <typename T, typename Comparer>
bool HeapCompare(HeapBase<T, Comparer>& aHeap, int aFirst, int aSecond)
{
    return aHeap.myComparer(aHeap.myData[aFirst], aHeap.myData[aSecond]);
}

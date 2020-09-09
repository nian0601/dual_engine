typedef unsigned long long DE_TimeUnit;

struct DE_Timer
{
    DE_TimeUnit myFrequency;
    DE_TimeUnit myCurrentTime;
    DE_TimeUnit myLastTime;
};

void UpdateTimer(DE_Timer& aTimer)
{
    aTimer.myLastTime = aTimer.myCurrentTime;
    
    LARGE_INTEGER largeInt;
    QueryPerformanceCounter(&largeInt);
    
    aTimer.myCurrentTime = largeInt.QuadPart * 1000000 / aTimer.myFrequency;
}

float GetDeltaTime(DE_Timer& aTimer)
{
    return min(0.1f, static_cast<float>(aTimer.myCurrentTime - aTimer.myLastTime) / 1000000.f);
}

DE_Timer GetTimer()
{
    LARGE_INTEGER largeInt;
    QueryPerformanceFrequency(&largeInt);
    
    DE_Timer timer;
    timer.myFrequency = largeInt.QuadPart;
    UpdateTimer(timer);
    timer.myLastTime = timer.myCurrentTime;
    
    return timer;
}

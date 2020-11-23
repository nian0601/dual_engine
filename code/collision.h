struct DE_Ray
{
    Vector3f myStart;
    Vector3f myEnd;
    float myThickness = 0.f;
};

struct DE_AABB
{
    Vector3f myCenterPos;
    Vector3f myExtents;
    Vector3f myMinPos;
    Vector3f myMaxPos;
};

DE_AABB AABBFromExtents(const Vector3f& aPosition, const Vector3f& someExtents)
{
    DE_AABB result;
    result.myCenterPos = aPosition;
    result.myExtents = someExtents;
    result.myMinPos = result.myCenterPos - result.myExtents;
    result.myMaxPos = result.myCenterPos + result.myExtents;
    return result;
}

DE_AABB AABBFromPoints(const Vector3f& aMin, const Vector3f& aMax)
{
    DE_AABB result;
    result.myMinPos = aMin;
    result.myMaxPos = aMax;
    result.myExtents = result.myMaxPos - result.myMinPos;
    result.myCenterPos = result.myMinPos + result.myExtents;
    return result;
}

struct DE_AABBi
{
    Vector3i myCenterPos;
    Vector3i myExtents;
    Vector3i myMinPos;
    Vector3i myMaxPos;
};

DE_AABBi AABBiFromExtents(const Vector3i& aPosition, const Vector3i& someExtents)
{
    DE_AABBi result;
    result.myCenterPos = aPosition;
    result.myExtents = someExtents;
    result.myMinPos = result.myCenterPos - result.myExtents;
    result.myMaxPos = result.myCenterPos + result.myExtents;
    return result;
}

DE_AABBi AABBiFromPoints(const Vector3i& aMin, const Vector3i& aMax)
{
    DE_AABBi result;
    result.myMinPos = aMin;
    result.myMaxPos = aMax;
    result.myExtents = result.myMaxPos - result.myMinPos;
    result.myCenterPos = result.myMinPos + result.myExtents;
    return result;
}
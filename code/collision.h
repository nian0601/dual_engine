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
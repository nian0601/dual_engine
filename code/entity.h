struct Entity
{
    Vector3f myPosition;
    Vector3f mySize;
    DE_AABB myAABB;
    bool myIsHighlighted;
    int myMapIndex;
    
    // Move into some componentsystem at some point..
    Vector3f myTargetPosition;
};

struct Entity
{
    Vector3f myPosition;
    Vector3f mySize;
    DE_AABB myAABB;
    bool myIsHighlighted;
    
    // Move into some componentsystem at some point..
    Vector3f myTargetPosition;
};

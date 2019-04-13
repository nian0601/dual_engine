void PlaceEntity(Entity& anEntity, const Vector3f& aPosition, const Vector3f& aSize)
{
    anEntity.myPosition = aPosition;
    anEntity.mySize = aSize;
    
    Vector3f aabbCenter = aPosition;
    aabbCenter.y += aSize.y * 0.5f;
    anEntity.myAABB = AABBFromExtents(aabbCenter, aSize*0.5f);
    
    anEntity.myTargetPosition = aPosition;
}

void PlaceEntityOnTop(Entity& anEntity, const Entity& aBottomEntity)
{
    Vector3f ontopPosition = aBottomEntity.myPosition;
    ontopPosition.y += aBottomEntity.mySize.y;
    
    ontopPosition.y += anEntity.mySize.y;
    PlaceEntity(anEntity, ontopPosition, anEntity.mySize);
}

Vector3f GetPositionOnTopOfEntity(const Entity& anEntity)
{
    Vector3f ontopPosition = anEntity.myPosition;
    ontopPosition.y += anEntity.mySize.y;
    return ontopPosition;
}

void RenderEntity(const Entity& anEntity, const Vector4f& aColor)
{
    QueueCube(anEntity.myPosition, anEntity.mySize, aColor);
}
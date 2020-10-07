bool AABBvsAABB(const DE_AABB& aFirst, const DE_AABB& aSecond)
{
    if(aFirst.myMaxPos.x < aSecond.myMinPos.x) return false;
    if(aFirst.myMaxPos.y < aSecond.myMinPos.y) return false;
    if(aFirst.myMaxPos.z < aSecond.myMinPos.z) return false;
    if(aFirst.myMinPos.x > aSecond.myMaxPos.x) return false;
    if(aFirst.myMinPos.y > aSecond.myMaxPos.y) return false;
    if(aFirst.myMinPos.z > aSecond.myMaxPos.z) return false;
    
    return true;
}

//Returns the squared distance to the collision point, -1 if no collision
float LineVSAABB(DE_AABB anAABB, const DE_Ray& aLine, Vector3f* aOutIntersectionPoint)
{
	Vector3f rayOrg = aLine.myStart;
	Vector3f rayDelta = aLine.myEnd - aLine.myStart;

    bool inside = true;

	float xt;
	float xn;

    anAABB.myMinPos -= aLine.myThickness * 0.5f;
    anAABB.myMaxPos += aLine.myThickness * 0.5f;
    
	if (rayOrg.x < anAABB.myMinPos.x)
	{
		xt = anAABB.myMinPos.x - rayOrg.x;
		if (xt > rayDelta.x)
		{
			return -1.f;
		}
		xt /= rayDelta.x;
		inside = false;
		xn = -1.f;
	}
	else if (rayOrg.x > anAABB.myMaxPos.x)
	{
		xt = anAABB.myMaxPos.x - rayOrg.x;
		if (xt < rayDelta.x)
		{
			return -1.f;
		}
		xt /= rayDelta.x;
		inside = false;
		xn = 1.f;
	}
	else
	{
		xt = -1.f;
	}
	float yt;
	float yn;

	if (rayOrg.y < anAABB.myMinPos.y)
	{
		yt = anAABB.myMinPos.y - rayOrg.y;
		if (yt > rayDelta.y)
		{
			return -1.f;
		}
		yt /= rayDelta.y;
		inside = false;
		yn = -1.f;
	}
	else if (rayOrg.y > anAABB.myMaxPos.y)
	{
		yt = anAABB.myMaxPos.y - rayOrg.y;
		if (yt < rayDelta.y)
		{
			return -1.f;
		}
		yt /= rayDelta.y;
		inside = false;
		yn = 1.f;
	}
	else
	{
		yt = -1.f;
	}

	float zt;
	float zn;

	if (rayOrg.z < anAABB.myMinPos.z)
	{
		zt = anAABB.myMinPos.z - rayOrg.z;
		if (zt > rayDelta.z)
		{
			return -1.f;
		}
		zt /= rayDelta.z;
		inside = false;
		zn = -1.f;
	}
	else if (rayOrg.z > anAABB.myMaxPos.z)
	{
		zt = anAABB.myMaxPos.z - rayOrg.z;
		if (zt < rayDelta.z)
		{
			return -1.f;
		}
		zt /= rayDelta.z;
		inside = false;
		zn = 1.f;
	}
	else
	{
		zt = -1.f;
	}
    
    Vector3f hitNormal;
	if (inside == true)
	{
		if(aOutIntersectionPoint)
			*aOutIntersectionPoint = aLine.myStart;
        
        hitNormal = {-rayDelta.x,-rayDelta.y, -rayDelta.z};
        
		return 0.f;
	}
    
	int which = 0;
    
	float t = xt;
    
	if (yt > t)
	{
		which = 1;
		t = yt;
	}
    
	if (zt > t)
	{
		which = 2;
		t = zt;
	}
    
	switch (which)
	{
        case 0: //intersecting the yz plane
        {
            float y = rayOrg.y + rayDelta.y * t;
            if (y < anAABB.myMinPos.y || y > anAABB.myMaxPos.y)
            {
                return -1.f;
            }
            float z = rayOrg.z + rayDelta.z * t;
            if (z < anAABB.myMinPos.z || z > anAABB.myMaxPos.z)
            {
                return -1.f;
            }
            
            hitNormal.x = xn;
            hitNormal.y = 0.f;
            hitNormal.z = 0.f;
            break;
        }
        case 1: //intersecting the xz plane
        {
            float x = rayOrg.x + rayDelta.x * t;
            if (x < anAABB.myMinPos.x || x > anAABB.myMaxPos.x)
            {
                return -1.f;
            }
            float z = rayOrg.z + rayDelta.z * t;
            if (z < anAABB.myMinPos.z || z > anAABB.myMaxPos.z)
            {
                return -1.f;
            }
            
            
            hitNormal.x = 0.f;
            hitNormal.y = yn;
            hitNormal.z = 0.f;
            break;
        }
        case 2: //intersecting the xy plane
        {
            float x = rayOrg.x + rayDelta.x * t;
            if (x < anAABB.myMinPos.x || x > anAABB.myMaxPos.x)
            {
                return -1.f;
            }
            float y = rayOrg.y + rayDelta.y * t;
            if (y < anAABB.myMinPos.y || y > anAABB.myMaxPos.y)
            {
                return -1.f;
            }
            
            
            hitNormal.x = 0.f;
            hitNormal.y = 0.f;
            hitNormal.z = zn;
            break;
        }
	}

	Vector3f intersectPoint = aLine.myStart + rayDelta * t;
	if (aOutIntersectionPoint)
		*aOutIntersectionPoint = intersectPoint;

	return Length2(intersectPoint - aLine.myStart);
}
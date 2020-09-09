#include "vector.cpp"
#include "matrix.cpp"
#include "quaternion.cpp"

#define FLT_MAX 3.402823466e+38F

Vector3f Unproject(const Vector2f& aPosition, float aDepth, const Matrix& aView, const Matrix& aProjection, const Vector2f& aWindowSize)
{
	float flippedY = aWindowSize.y - aPosition.y;

	Vector4f in;
	in.x = aPosition.x / aWindowSize.x * 2.f - 1.f;
	in.y = flippedY / aWindowSize.y * 2.f - 1.f;
	in.z = 2.f * aDepth - 1.f;
	in.w = 1.f;

	Matrix invertedViewProjection = InverseReal(aView * aProjection);

	Vector4f worldPos = in * invertedViewProjection;
	if (worldPos.w == 0.f)
		return Vector3f();

	worldPos.w = 1.f / worldPos.w;

	Vector3f out;
	out.x = worldPos.x * worldPos.w;
	out.y = worldPos.y * worldPos.w;
	out.z = worldPos.z * worldPos.w;
	return out;
}

int RandomInt(int aMin, int aMax)
{
    return (rand() % (aMax - aMin + 1)) + aMin;
}

float Random01()
{
    return (rand() % 10000) / 10000.f;
}

int Round(float aNumber)
{
    if(aNumber > 0.f)
        return static_cast<int>(aNumber + 0.5f);
    
    return static_cast<int>(aNumber - 0.5f);
}
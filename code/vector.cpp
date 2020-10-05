
// Vector4

struct Vector4f
{
    float x;
    float y;
    float z;
    float w;
};

bool operator==(const Vector4f& aA, const Vector4f& aB)
{
    return aA.x == aB.x && aA.y == aB.y && aA.z == aB.z && aA.w == aB.w;
}

bool operator!=(const Vector4f& aA, const Vector4f& aB)
{
    return !(aA == aB);
}

// Addition
Vector4f operator+=(Vector4f& aA, float aScalar)
{
    aA.x += aScalar;
    aA.y += aScalar;
    aA.z += aScalar;
    aA.w += aScalar;
    return aA;
}

Vector4f operator+(const Vector4f& aA, float aScalar)
{
    Vector4f result = aA;
    result += aScalar;
    return result;
}

Vector4f operator+=(Vector4f& aA, const Vector4f& aB)
{
    aA.x += aB.x;
    aA.y += aB.y;
    aA.z += aB.z;
    aA.w += aB.w;
    return aA;
}

Vector4f operator+(const Vector4f& aA, const Vector4f& aB)
{
    Vector4f result = aA;
    result += aB;
    return result;
}

// Subtraction
Vector4f operator-=(Vector4f& aA, float aScalar)
{
    aA.x -= aScalar;
    aA.y -= aScalar;
    aA.z -= aScalar;
    aA.w -= aScalar;
    return aA;
}

Vector4f operator-(const Vector4f& aA, float aScalar)
{
    Vector4f result = aA;
    result -= aScalar;
    return result;
}

Vector4f operator-=(Vector4f& aA, const Vector4f& aB)
{
    aA.x -= aB.x;
    aA.y -= aB.y;
    aA.z -= aB.z;
    aA.w -= aB.w;
    return aA;
}

Vector4f operator-(const Vector4f& aA, const Vector4f& aB)
{
    Vector4f result = aA;
    result -= aB;
    return result;
}

// Multiplication
Vector4f operator*=(Vector4f& aA, float aScalar)
{
    aA.x *= aScalar;
    aA.y *= aScalar;
    aA.z *= aScalar;
    aA.w *= aScalar;
    return aA;
}

Vector4f operator*(const Vector4f& aA, float aScalar)
{
    Vector4f result = aA;
    result *= aScalar;
    return result;
}

Vector4f operator*=(Vector4f& aA, const Vector4f& aB)
{
    aA.x *= aB.x;
    aA.y *= aB.y;
    aA.z *= aB.z;
    aA.w *= aB.w;
    return aA;
}

Vector4f operator*(const Vector4f& aA, const Vector4f& aB)
{
    Vector4f result = aA;
    result *= aB;
    return result;
}

// Division
Vector4f operator/=(Vector4f& aA, float aScalar)
{
    aA.x /= aScalar;
    aA.y /= aScalar;
    aA.z /= aScalar;
    aA.w /= aScalar;
    return aA;
}

Vector4f operator/(const Vector4f& aA, float aScalar)
{
    Vector4f result = aA;
    result /= aScalar;
    return result;
}

Vector4f operator/=(Vector4f& aA, const Vector4f& aB)
{
    aA.x /= aB.x;
    aA.y /= aB.y;
    aA.z /= aB.z;
    aA.w /= aB.w;
    return aA;
}

Vector4f operator/(const Vector4f& aA, const Vector4f& aB)
{
    Vector4f result = aA;
    result /= aB;
    return result;
}

float Dot(const Vector4f& aFirst, const Vector4f& aSecond)
{
    return aFirst.x * aSecond.x + aFirst.y * aSecond.y + aFirst.z * aSecond.z + aFirst.w * aSecond.w;
}

float Length2(const Vector4f& aVector)
{
    return Dot(aVector, aVector);
}

float Length(const Vector4f& aVector)
{
    return sqrt(Length2(aVector));
}

void Normalize(Vector4f& aVector)
{
    float length = Length(aVector);
    if(length == 0.f)
        return;
    
    aVector /= length;
}

// Vector3

struct Vector3f
{
    float x;
    float y;
    float z;
};

bool operator==(const Vector3f& aA, const Vector3f& aB)
{
    return aA.x == aB.x && aA.y == aB.y && aA.z == aB.z;
}

bool operator!=(const Vector3f& aA, const Vector3f& aB)
{
    return !(aA == aB);
}

// Addition
Vector3f operator+=(Vector3f& aA, float aScalar)
{
    aA.x += aScalar;
    aA.y += aScalar;
    aA.z += aScalar;
    return aA;
}

Vector3f operator+(const Vector3f& aA, float aScalar)
{
    Vector3f result = aA;
    result += aScalar;
    return result;
}

Vector3f operator+=(Vector3f& aA, const Vector3f& aB)
{
    aA.x += aB.x;
    aA.y += aB.y;
    aA.z += aB.z;
    return aA;
}

Vector3f operator+(const Vector3f& aA, const Vector3f& aB)
{
    Vector3f result = aA;
    result += aB;
    return result;
}

// Subtraction
Vector3f operator-=(Vector3f& aA, float aScalar)
{
    aA.x -= aScalar;
    aA.y -= aScalar;
    aA.z -= aScalar;
    return aA;
}

Vector3f operator-(const Vector3f& aA, float aScalar)
{
    Vector3f result = aA;
    result -= aScalar;
    return result;
}

Vector3f operator-=(Vector3f& aA, const Vector3f& aB)
{
    aA.x -= aB.x;
    aA.y -= aB.y;
    aA.z -= aB.z;
    return aA;
}

Vector3f operator-(const Vector3f& aA, const Vector3f& aB)
{
    Vector3f result = aA;
    result -= aB;
    return result;
}

// Multiplication
Vector3f operator*=(Vector3f& aA, float aScalar)
{
    aA.x *= aScalar;
    aA.y *= aScalar;
    aA.z *= aScalar;
    return aA;
}

Vector3f operator*(const Vector3f& aA, float aScalar)
{
    Vector3f result = aA;
    result *= aScalar;
    return result;
}

Vector3f operator*=(Vector3f& aA, const Vector3f& aB)
{
    aA.x *= aB.x;
    aA.y *= aB.y;
    aA.z *= aB.z;
    return aA;
}

Vector3f operator*(const Vector3f& aA, const Vector3f& aB)
{
    Vector3f result = aA;
    result *= aB;
    return result;
}

// Division
Vector3f operator/=(Vector3f& aA, float aScalar)
{
    aA.x /= aScalar;
    aA.y /= aScalar;
    aA.z /= aScalar;
    return aA;
}

Vector3f operator/(const Vector3f& aA, float aScalar)
{
    Vector3f result = aA;
    result /= aScalar;
    return result;
}

Vector3f operator/=(Vector3f& aA, const Vector3f& aB)
{
    aA.x /= aB.x;
    aA.y /= aB.y;
    aA.z /= aB.z;
    return aA;
}

Vector3f operator/(const Vector3f& aA, const Vector3f& aB)
{
    Vector3f result = aA;
    result /= aB;
    return result;
}

float Dot(const Vector3f& aFirst, const Vector3f& aSecond)
{
    return aFirst.x * aSecond.x + aFirst.y * aSecond.y + aFirst.z * aSecond.z;
}

Vector3f Cross(const Vector3f& aFirst, const Vector3f& aSecond)
{
    Vector3f result;
    result.x = aFirst.y * aSecond.z - aFirst.z * aSecond.y;
    result.y = aFirst.z * aSecond.x - aFirst.x * aSecond.z;
    result.z = aFirst.x * aSecond.y - aFirst.y * aSecond.x;
    return result;
}

float Length2(const Vector3f& aVector)
{
    return Dot(aVector, aVector);
}

float Length(const Vector3f& aVector)
{
    return sqrt(Length2(aVector));
}


void Normalize(Vector3f& aVector)
{
    float length = Length(aVector);
    if(length == 0.f)
        return;
    
    aVector /= length;
}
// Vector2

struct Vector2f
{
    float x;
    float y;
};

bool operator==(const Vector2f& aA, const Vector2f& aB)
{
    return aA.x == aB.x && aA.y == aB.y;
}

bool operator!=(const Vector2f& aA, const Vector2f& aB)
{
    return !(aA == aB);
}

// Addition
Vector2f operator+=(Vector2f& aA, float aScalar)
{
    aA.x += aScalar;
    aA.y += aScalar;
    return aA;
}

Vector2f operator+(const Vector2f& aA, float aScalar)
{
    Vector2f result = aA;
    result += aScalar;
    return result;
}

Vector2f operator+=(Vector2f& aA, const Vector2f& aB)
{
    aA.x += aB.x;
    aA.y += aB.y;
    return aA;
}

Vector2f operator+(const Vector2f& aA, const Vector2f& aB)
{
    Vector2f result = aA;
    result += aB;
    return result;
}

// Subtraction
Vector2f operator-=(Vector2f& aA, float aScalar)
{
    aA.x -= aScalar;
    aA.y -= aScalar;
    return aA;
}

Vector2f operator-(const Vector2f& aA, float aScalar)
{
    Vector2f result = aA;
    result -= aScalar;
    return result;
}

Vector2f operator-=(Vector2f& aA, const Vector2f& aB)
{
    aA.x -= aB.x;
    aA.y -= aB.y;
    return aA;
}

Vector2f operator-(const Vector2f& aA, const Vector2f& aB)
{
    Vector2f result = aA;
    result -= aB;
    return result;
}

// Multiplication
Vector2f operator*=(Vector2f& aA, float aScalar)
{
    aA.x *= aScalar;
    aA.y *= aScalar;
    return aA;
}

Vector2f operator*(const Vector2f& aA, float aScalar)
{
    Vector2f result = aA;
    result *= aScalar;
    return result;
}

Vector2f operator*=(Vector2f& aA, const Vector2f& aB)
{
    aA.x *= aB.x;
    aA.y *= aB.y;
    return aA;
}

Vector2f operator*(const Vector2f& aA, const Vector2f& aB)
{
    Vector2f result = aA;
    result *= aB;
    return result;
}

// Division
Vector2f operator/=(Vector2f& aA, float aScalar)
{
    aA.x /= aScalar;
    aA.y /= aScalar;
    return aA;
}

Vector2f operator/(const Vector2f& aA, float aScalar)
{
    Vector2f result = aA;
    result /= aScalar;
    return result;
}

Vector2f operator/=(Vector2f& aA, const Vector2f& aB)
{
    aA.x /= aB.x;
    aA.y /= aB.y;
    return aA;
}

Vector2f operator/(const Vector2f& aA, const Vector2f& aB)
{
    Vector2f result = aA;
    result /= aB;
    return result;
}

float Dot(const Vector2f& aFirst, const Vector2f& aSecond)
{
    return aFirst.x * aSecond.x + aFirst.y * aSecond.y;
}

float Length2(const Vector2f& aVector)
{
    return Dot(aVector, aVector);
}

float Length(const Vector2f& aVector)
{
    return sqrt(Length2(aVector));
}

void Normalize(Vector2f& aVector)
{
    float length = Length(aVector);
    if(length == 0.f)
        return;
    
    aVector /= length;
}
struct Quaternion
{
    float x;
    float y;
    float z;
    float w;
};

Quaternion MakeQuaternion(const Vector3f& aV, float aRadians)
{
    float axisCoef = sin(aRadians * 0.5f);
    
    Quaternion quat;
    quat.x = axisCoef * aV.x;
    quat.y = axisCoef * aV.y;
    quat.z = axisCoef * aV.z;
    quat.w = cos(aRadians * 0.5f);
    return quat;
}

Matrix RotationMatrixAxisAngle(const Vector3f& aAxis, float aRadians)
{
    Quaternion quat = MakeQuaternion(aAxis, aRadians);
    
    float x = quat.x;
    float y = quat.y;
    float z = quat.z;
    float w = quat.w;
    
    Matrix matrix = IdentityMatrix();
    matrix.myData[0] = 1.f - (2.f * (y * y)) - (2.f * (z * z));
    matrix.myData[1] = (2.f * x * y) + (2.f * w * z);
    matrix.myData[2] = (2.f * x * z) - (2.f * w * y);
    
    matrix.myData[4] = (2.f * x * y) - (2.f * w * z);
    matrix.myData[5] = 1.f - (2.f * (x * x)) - (2.f * (z * z));
    matrix.myData[6] = (2.f * y * z) + (2.f * w * x);
    
    matrix.myData[8] = (2.f * x * z) + (2.f * w * y);
    matrix.myData[9] = (2.f * y * z) - (2.f * w * x);
    matrix.myData[10] = 1.f - (2.f * (x * x)) - (2.f * (y * y));
    
    return matrix;
}
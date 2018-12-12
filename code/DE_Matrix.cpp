struct Matrix
{
    union
    {
        float myData[4 * 4];
        
        struct row
        {
            Vector3f myData;
            float myW;
        };
        row myRows[4];
    };
};

Matrix IdentityMatrix()
{
    Matrix result = {};
    result.myData[0] = 1.f;
    result.myData[5] = 1.f;
    result.myData[10] = 1.f;
    result.myData[15] = 1.f;
    return result;
}

Matrix RotationMatrixX(float aRadian)
{
    Matrix result = IdentityMatrix();
    result.myData[5] = cos(aRadian);
    result.myData[5] = sin(aRadian);
    result.myData[9] = -sin(aRadian);
    result.myData[10] = cos(aRadian);
    return result;
}

Matrix RotationMatrixY(float aRadian)
{
    Matrix result = IdentityMatrix();
    result.myData[0] = cos(aRadian);
    result.myData[2] = -sin(aRadian);
    result.myData[8] = sin(aRadian);
    result.myData[10] = cos(aRadian);
    
    return result;
}

Matrix RotationMatrixZ(float aRadian)
{
    Matrix result = IdentityMatrix();
    result.myData[0] = cos(aRadian);
    result.myData[1] = sin(aRadian);
    result.myData[4] = -sin(aRadian);
    result.myData[5] = cos(aRadian);
    return result;
}

Matrix ProjectionMatrix(float aNearZ, float aFarZ, float aAspectRatio, float aFovAngle)
{
	float SinFov = sin(0.5f * aFovAngle);
	float CosFov = cos(0.5f * aFovAngle);
    
	float scaling = aFarZ / (aFarZ - aNearZ);

    Matrix result = {};
	result.myData[0] = CosFov / SinFov;
	result.myData[5] = (CosFov / SinFov) / aAspectRatio;
	result.myData[10] = scaling;
	result.myData[11] = 1.0f;

	result.myData[14] = -scaling * aNearZ;
	result.myData[15] = 0.0f;
	return result;
}

Matrix OrthagonalMatrix(float aWidth, float aHeight, float aNearZ, float aFarZ)
{
	Matrix result;

	result.myData[0] = 2.f / aWidth;
	result.myData[5] = 2.f / aHeight;
	result.myData[10] = 1.f / (aFarZ - aNearZ);
	result.myData[14] = aNearZ / (aNearZ - aFarZ);
	result.myData[15] = 1.f;

	return result;
}


void Translate(Matrix& aMatrix, const Vector3f& aTranslation)
{
    aMatrix.myRows[3].myData += aTranslation;
}

Matrix operator*=(Matrix& aA, const Matrix& aB)
{
    Matrix original = aA;
    
    aA.myData[0] =
		original.myData[0] * aB.myData[0] +
		original.myData[1] * aB.myData[4] +
		original.myData[2] * aB.myData[8] +
		original.myData[3] * aB.myData[12];
	aA.myData[1] =
		original.myData[0] * aB.myData[1] +
		original.myData[1] * aB.myData[5] +
		original.myData[2] * aB.myData[9] +
		original.myData[3] * aB.myData[13];
	aA.myData[2] =
		original.myData[0] * aB.myData[2] +
		original.myData[1] * aB.myData[6] +
		original.myData[2] * aB.myData[10] +
		original.myData[3] * aB.myData[14];
	aA.myData[3] =
		original.myData[0] * aB.myData[3] +
		original.myData[1] * aB.myData[7] +
		original.myData[2] * aB.myData[11] +
		original.myData[3] * aB.myData[15];

	aA.myData[4] =
		original.myData[4] * aB.myData[0] +
		original.myData[5] * aB.myData[4] +
		original.myData[6] * aB.myData[8] +
		original.myData[7] * aB.myData[12];
	aA.myData[5] =
		original.myData[4] * aB.myData[1] +
		original.myData[5] * aB.myData[5] +
		original.myData[6] * aB.myData[9] +
		original.myData[7] * aB.myData[13];
	aA.myData[6] =
		original.myData[4] * aB.myData[2] +
		original.myData[5] * aB.myData[6] +
		original.myData[6] * aB.myData[10] +
		original.myData[7] * aB.myData[14];
	aA.myData[7] =
		original.myData[4] * aB.myData[3] +
		original.myData[5] * aB.myData[7] +
		original.myData[6] * aB.myData[11] +
		original.myData[7] * aB.myData[15];

	aA.myData[8] =
		original.myData[8] * aB.myData[0] +
		original.myData[9] * aB.myData[4] +
		original.myData[10] * aB.myData[8] +
		original.myData[11] * aB.myData[12];
	aA.myData[9] =
		original.myData[8] * aB.myData[1] +
		original.myData[9] * aB.myData[5] +
		original.myData[10] * aB.myData[9] +
		original.myData[11] * aB.myData[13];
	aA.myData[10] =
		original.myData[8] * aB.myData[2] +
		original.myData[9] * aB.myData[6] +
		original.myData[10] * aB.myData[10] +
		original.myData[11] * aB.myData[14];
	aA.myData[11] =
		original.myData[8] * aB.myData[3] +
		original.myData[9] * aB.myData[7] +
		original.myData[10] * aB.myData[11] +
		original.myData[11] * aB.myData[15];

	aA.myData[12] =
		original.myData[12] * aB.myData[0] +
		original.myData[13] * aB.myData[4] +
		original.myData[14] * aB.myData[8] +
		original.myData[15] * aB.myData[12];
	aA.myData[13] =
		original.myData[12] * aB.myData[1] +
		original.myData[13] * aB.myData[5] +
		original.myData[14] * aB.myData[9] +
		original.myData[15] * aB.myData[13];
	aA.myData[14] =
		original.myData[12] * aB.myData[2] +
		original.myData[13] * aB.myData[6] +
		original.myData[14] * aB.myData[10] +
		original.myData[15] * aB.myData[14];
	aA.myData[15] =
		original.myData[12] * aB.myData[3] +
		original.myData[13] * aB.myData[7] +
		original.myData[14] * aB.myData[11] +
		original.myData[15] * aB.myData[15];
    
    return aA;
}

Matrix operator*(const Matrix& aA, const Matrix& aB)
{
    Matrix result = aA;
    result *= aB;
    return result;
}

Vector3f operator*(const Vector3f& aVector, const Matrix& aMatrix)
{
    Vector3f result;
    result.x = aVector.x * aMatrix.myData[0] + aVector.y * aMatrix.myData[4] + aVector.z * aMatrix.myData[8];
    result.y = aVector.x * aMatrix.myData[1] + aVector.y * aMatrix.myData[5] + aVector.z * aMatrix.myData[9];
    result.z = aVector.x * aMatrix.myData[2] + aVector.y * aMatrix.myData[6] + aVector.z * aMatrix.myData[10];
    return result;
}
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

Vector4f operator*(const Vector4f& aVector, const Matrix& aMatrix)
{
    Vector4f result;
    result.x = aVector.x * aMatrix.myData[0] + aVector.y * aMatrix.myData[4] + aVector.z * aMatrix.myData[8] + aVector.w * aMatrix.myData[12];
    result.y = aVector.x * aMatrix.myData[1] + aVector.y * aMatrix.myData[5] + aVector.z * aMatrix.myData[9] + aVector.w * aMatrix.myData[13];
    result.z = aVector.x * aMatrix.myData[2] + aVector.y * aMatrix.myData[6] + aVector.z * aMatrix.myData[10] + aVector.w * aMatrix.myData[14];
    result.w = aVector.x * aMatrix.myData[3] + aVector.y * aMatrix.myData[7] + aVector.z * aMatrix.myData[11] + aVector.w * aMatrix.myData[15];
    return result;
}

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
    result.myData[6] = sin(aRadian);
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

Matrix ScaleMatrix(const Vector3f& aScale)
{
    Matrix result = IdentityMatrix();
    result.myData[0] = aScale.x;
    result.myData[5] = aScale.y;
    result.myData[10] = aScale.z;
    return result;
}

// aAspectRatio should be height over width
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


// aAspectRatio should be width over height
Matrix OpenGLProjectionMatrix(float aNearZ, float aFarZ, float aAspectRatio, float aFovAngle)
{
	float tanHalfFovy = tan(aFovAngle / 2.f);
    
    Matrix result = {};
    result.myData[0] = 1.f / (aAspectRatio * tanHalfFovy);
    result.myData[5] = 1.f / tanHalfFovy;
    result.myData[10] = (aFarZ + aNearZ) / (aFarZ - aNearZ);
    result.myData[11] = 1.f;
    result.myData[14] = -(2.f * aFarZ * aNearZ) / (aFarZ - aNearZ);
    
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

void Transpose(Matrix& aMatrix)
{
    Matrix temp = aMatrix;
    
	aMatrix.myData[1] = temp.myData[4];
	aMatrix.myData[2] = temp.myData[8];
	aMatrix.myData[3] = temp.myData[12];
	aMatrix.myData[7] = temp.myData[13];
	aMatrix.myData[11] = temp.myData[14];
	aMatrix.myData[6] = temp.myData[9];
    
	aMatrix.myData[4] = temp.myData[1];
	aMatrix.myData[8] = temp.myData[2];
	aMatrix.myData[12] = temp.myData[3];
	aMatrix.myData[13] = temp.myData[7];
	aMatrix.myData[14] = temp.myData[11];
	aMatrix.myData[9] = temp.myData[6];
}

Vector4f GetTranslation(const Matrix& aMatrix)
{
    Vector4f result;
    result.x = aMatrix.myRows[3].myData.x;
    result.y = aMatrix.myRows[3].myData.y;
    result.z = aMatrix.myRows[3].myData.z;
    result.w = 1.f;
    return result;
}

void Translate(Matrix& aMatrix, const Vector3f& aTranslation)
{
    aMatrix.myRows[3].myData += aTranslation;
}

void SetTranslation(Matrix& aMatrix, const Vector3f& aTranslation)
{
    aMatrix.myRows[3].myData = aTranslation;
}

void TranslateRight(Matrix& aMatrix, float aDistance)
{
    Translate(aMatrix, aMatrix.myRows[0].myData * aDistance);
}

void TranslateUp(Matrix& aMatrix, float aDistance)
{
    Translate(aMatrix, aMatrix.myRows[1].myData * aDistance);
}

void TranslateForward(Matrix& aMatrix, float aDistance)
{
    Translate(aMatrix, aMatrix.myRows[2].myData * aDistance);
}

Matrix InverseSimple(const Matrix& aMatrix)
{
    Matrix result = aMatrix;
    
    Vector4f position = GetTranslation(result);
    result.myRows[3] = {0.f, 0.f, 0.f, 1.f};
    
    position *= -1.f;
    position.w = 1.f;
    
    Transpose(result);
    position = position * result;
    
    Translate(result, {position.x, position.y, position.z});
    
    return result;
}

Matrix InverseReal(const Matrix& aMatrix)
{
	float inv[16], det;
	int i;

	inv[0] = aMatrix.myData[5] * aMatrix.myData[10] * aMatrix.myData[15] -
		aMatrix.myData[5] * aMatrix.myData[11] * aMatrix.myData[14] -
		aMatrix.myData[9] * aMatrix.myData[6] * aMatrix.myData[15] +
		aMatrix.myData[9] * aMatrix.myData[7] * aMatrix.myData[14] +
		aMatrix.myData[13] * aMatrix.myData[6] * aMatrix.myData[11] -
		aMatrix.myData[13] * aMatrix.myData[7] * aMatrix.myData[10];

	inv[4] = -aMatrix.myData[4] * aMatrix.myData[10] * aMatrix.myData[15] +
		aMatrix.myData[4] * aMatrix.myData[11] * aMatrix.myData[14] +
		aMatrix.myData[8] * aMatrix.myData[6] * aMatrix.myData[15] -
		aMatrix.myData[8] * aMatrix.myData[7] * aMatrix.myData[14] -
		aMatrix.myData[12] * aMatrix.myData[6] * aMatrix.myData[11] +
		aMatrix.myData[12] * aMatrix.myData[7] * aMatrix.myData[10];

	inv[8] = aMatrix.myData[4] * aMatrix.myData[9] * aMatrix.myData[15] -
		aMatrix.myData[4] * aMatrix.myData[11] * aMatrix.myData[13] -
		aMatrix.myData[8] * aMatrix.myData[5] * aMatrix.myData[15] +
		aMatrix.myData[8] * aMatrix.myData[7] * aMatrix.myData[13] +
		aMatrix.myData[12] * aMatrix.myData[5] * aMatrix.myData[11] -
		aMatrix.myData[12] * aMatrix.myData[7] * aMatrix.myData[9];

	inv[12] = -aMatrix.myData[4] * aMatrix.myData[9] * aMatrix.myData[14] +
		aMatrix.myData[4] * aMatrix.myData[10] * aMatrix.myData[13] +
		aMatrix.myData[8] * aMatrix.myData[5] * aMatrix.myData[14] -
		aMatrix.myData[8] * aMatrix.myData[6] * aMatrix.myData[13] -
		aMatrix.myData[12] * aMatrix.myData[5] * aMatrix.myData[10] +
		aMatrix.myData[12] * aMatrix.myData[6] * aMatrix.myData[9];

	inv[1] = -aMatrix.myData[1] * aMatrix.myData[10] * aMatrix.myData[15] +
		aMatrix.myData[1] * aMatrix.myData[11] * aMatrix.myData[14] +
		aMatrix.myData[9] * aMatrix.myData[2] * aMatrix.myData[15] -
		aMatrix.myData[9] * aMatrix.myData[3] * aMatrix.myData[14] -
		aMatrix.myData[13] * aMatrix.myData[2] * aMatrix.myData[11] +
		aMatrix.myData[13] * aMatrix.myData[3] * aMatrix.myData[10];

	inv[5] = aMatrix.myData[0] * aMatrix.myData[10] * aMatrix.myData[15] -
		aMatrix.myData[0] * aMatrix.myData[11] * aMatrix.myData[14] -
		aMatrix.myData[8] * aMatrix.myData[2] * aMatrix.myData[15] +
		aMatrix.myData[8] * aMatrix.myData[3] * aMatrix.myData[14] +
		aMatrix.myData[12] * aMatrix.myData[2] * aMatrix.myData[11] -
		aMatrix.myData[12] * aMatrix.myData[3] * aMatrix.myData[10];

	inv[9] = -aMatrix.myData[0] * aMatrix.myData[9] * aMatrix.myData[15] +
		aMatrix.myData[0] * aMatrix.myData[11] * aMatrix.myData[13] +
		aMatrix.myData[8] * aMatrix.myData[1] * aMatrix.myData[15] -
		aMatrix.myData[8] * aMatrix.myData[3] * aMatrix.myData[13] -
		aMatrix.myData[12] * aMatrix.myData[1] * aMatrix.myData[11] +
		aMatrix.myData[12] * aMatrix.myData[3] * aMatrix.myData[9];

	inv[13] = aMatrix.myData[0] * aMatrix.myData[9] * aMatrix.myData[14] -
		aMatrix.myData[0] * aMatrix.myData[10] * aMatrix.myData[13] -
		aMatrix.myData[8] * aMatrix.myData[1] * aMatrix.myData[14] +
		aMatrix.myData[8] * aMatrix.myData[2] * aMatrix.myData[13] +
		aMatrix.myData[12] * aMatrix.myData[1] * aMatrix.myData[10] -
		aMatrix.myData[12] * aMatrix.myData[2] * aMatrix.myData[9];

	inv[2] = aMatrix.myData[1] * aMatrix.myData[6] * aMatrix.myData[15] -
		aMatrix.myData[1] * aMatrix.myData[7] * aMatrix.myData[14] -
		aMatrix.myData[5] * aMatrix.myData[2] * aMatrix.myData[15] +
		aMatrix.myData[5] * aMatrix.myData[3] * aMatrix.myData[14] +
		aMatrix.myData[13] * aMatrix.myData[2] * aMatrix.myData[7] -
		aMatrix.myData[13] * aMatrix.myData[3] * aMatrix.myData[6];

	inv[6] = -aMatrix.myData[0] * aMatrix.myData[6] * aMatrix.myData[15] +
		aMatrix.myData[0] * aMatrix.myData[7] * aMatrix.myData[14] +
		aMatrix.myData[4] * aMatrix.myData[2] * aMatrix.myData[15] -
		aMatrix.myData[4] * aMatrix.myData[3] * aMatrix.myData[14] -
		aMatrix.myData[12] * aMatrix.myData[2] * aMatrix.myData[7] +
		aMatrix.myData[12] * aMatrix.myData[3] * aMatrix.myData[6];

	inv[10] = aMatrix.myData[0] * aMatrix.myData[5] * aMatrix.myData[15] -
		aMatrix.myData[0] * aMatrix.myData[7] * aMatrix.myData[13] -
		aMatrix.myData[4] * aMatrix.myData[1] * aMatrix.myData[15] +
		aMatrix.myData[4] * aMatrix.myData[3] * aMatrix.myData[13] +
		aMatrix.myData[12] * aMatrix.myData[1] * aMatrix.myData[7] -
		aMatrix.myData[12] * aMatrix.myData[3] * aMatrix.myData[5];

	inv[14] = -aMatrix.myData[0] * aMatrix.myData[5] * aMatrix.myData[14] +
		aMatrix.myData[0] * aMatrix.myData[6] * aMatrix.myData[13] +
		aMatrix.myData[4] * aMatrix.myData[1] * aMatrix.myData[14] -
		aMatrix.myData[4] * aMatrix.myData[2] * aMatrix.myData[13] -
		aMatrix.myData[12] * aMatrix.myData[1] * aMatrix.myData[6] +
		aMatrix.myData[12] * aMatrix.myData[2] * aMatrix.myData[5];

	inv[3] = -aMatrix.myData[1] * aMatrix.myData[6] * aMatrix.myData[11] +
		aMatrix.myData[1] * aMatrix.myData[7] * aMatrix.myData[10] +
		aMatrix.myData[5] * aMatrix.myData[2] * aMatrix.myData[11] -
		aMatrix.myData[5] * aMatrix.myData[3] * aMatrix.myData[10] -
		aMatrix.myData[9] * aMatrix.myData[2] * aMatrix.myData[7] +
		aMatrix.myData[9] * aMatrix.myData[3] * aMatrix.myData[6];

	inv[7] = aMatrix.myData[0] * aMatrix.myData[6] * aMatrix.myData[11] -
		aMatrix.myData[0] * aMatrix.myData[7] * aMatrix.myData[10] -
		aMatrix.myData[4] * aMatrix.myData[2] * aMatrix.myData[11] +
		aMatrix.myData[4] * aMatrix.myData[3] * aMatrix.myData[10] +
		aMatrix.myData[8] * aMatrix.myData[2] * aMatrix.myData[7] -
		aMatrix.myData[8] * aMatrix.myData[3] * aMatrix.myData[6];

	inv[11] = -aMatrix.myData[0] * aMatrix.myData[5] * aMatrix.myData[11] +
		aMatrix.myData[0] * aMatrix.myData[7] * aMatrix.myData[9] +
		aMatrix.myData[4] * aMatrix.myData[1] * aMatrix.myData[11] -
		aMatrix.myData[4] * aMatrix.myData[3] * aMatrix.myData[9] -
		aMatrix.myData[8] * aMatrix.myData[1] * aMatrix.myData[7] +
		aMatrix.myData[8] * aMatrix.myData[3] * aMatrix.myData[5];

	inv[15] = aMatrix.myData[0] * aMatrix.myData[5] * aMatrix.myData[10] -
		aMatrix.myData[0] * aMatrix.myData[6] * aMatrix.myData[9] -
		aMatrix.myData[4] * aMatrix.myData[1] * aMatrix.myData[10] +
		aMatrix.myData[4] * aMatrix.myData[2] * aMatrix.myData[9] +
		aMatrix.myData[8] * aMatrix.myData[1] * aMatrix.myData[6] -
		aMatrix.myData[8] * aMatrix.myData[2] * aMatrix.myData[5];

	det = aMatrix.myData[0] * inv[0] + aMatrix.myData[1] * inv[4] + aMatrix.myData[2] * inv[8] + aMatrix.myData[3] * inv[12];

	det = 1.0f / det;

	Matrix returnMatrix;

	for (i = 0; i < 16; i++)
		returnMatrix.myData[i] = inv[i] * det;

	return returnMatrix;
}

void RotateX(Matrix& aMatrix, float aRadians)
{
    Vector4f translation = GetTranslation(aMatrix);
    SetTranslation(aMatrix, {0.f, 0.f, 0.f});
    
    aMatrix = aMatrix * RotationMatrixX(aRadians);
    
    SetTranslation(aMatrix, {translation.x, translation.y, translation.z});
}

void RotateY(Matrix& aMatrix, float aRadians)
{
    Vector4f translation = GetTranslation(aMatrix);
    SetTranslation(aMatrix, {0.f, 0.f, 0.f});
    
    aMatrix = aMatrix * RotationMatrixY(aRadians);
    
    SetTranslation(aMatrix, {translation.x, translation.y, translation.z});
}

void RotateZ(Matrix& aMatrix, float aRadians)
{
    Vector4f translation = GetTranslation(aMatrix);
    SetTranslation(aMatrix, {0.f, 0.f, 0.f});
    
    aMatrix = aMatrix * RotationMatrixZ(aRadians);
    
    SetTranslation(aMatrix, {translation.x, translation.y, translation.z});
}
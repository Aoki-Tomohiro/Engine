struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 color : COLOR0;
    float32_t3 worldPosition : POSITION0;
    float32_t3 toEye : POSITION1;
    float32_t3 cameraToPosition : POSITION2;
};

static const uint32_t kMaxParticles = 1024;

struct Particle
{
    float32_t3 translate;
    float32_t3 rotate;
    float32_t4 quaternion;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
    int32_t alignToDirection;
    float32_t3 initialColor;
    float32_t3 targetColor;
    float32_t initialAlpha;
    float32_t targetAlpha;
    float32_t3 initialScale;
    float32_t3 targetScale;
    float32_t3 rotSpeed;
};

struct PerFrame
{
    float32_t time;
    float32_t deltaTime;
};

float32_t4x4 MakeRotateXMatrix(float radian)
{
    float32_t4x4 result;
    result[0][0] = 1;
    result[0][1] = 0;
    result[0][2] = 0;
    result[0][3] = 0;

    result[1][0] = 0;
    result[1][1] = cos(radian);
    result[1][2] = sin(radian);
    result[1][3] = 0;

    result[2][0] = 0;
    result[2][1] = -sin(radian);
    result[2][2] = cos(radian);
    result[2][3] = 0;

    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;

    return result;
}


float32_t4x4 MakeRotateYMatrix(float radian)
{
    float32_t4x4 result;
    result[0][0] = cos(radian);
    result[0][1] = 0;
    result[0][2] = -sin(radian);
    result[0][3] = 0;

    result[1][0] = 0;
    result[1][1] = 1;
    result[1][2] = 0;
    result[1][3] = 0;

    result[2][0] = sin(radian);
    result[2][1] = 0;
    result[2][2] = cos(radian);
    result[2][3] = 0;

    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;

    return result;
}


float32_t4x4 MakeRotateZMatrix(float radian)
{
    float32_t4x4 result;
    result[0][0] = cos(radian);
    result[0][1] = sin(radian);
    result[0][2] = 0;
    result[0][3] = 0;

    result[1][0] = -sin(radian);
    result[1][1] = cos(radian);
    result[1][2] = 0;
    result[1][3] = 0;

    result[2][0] = 0;
    result[2][1] = 0;
    result[2][2] = 1;
    result[2][3] = 0;

    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;

    return result;
}


float32_t4x4 MakeRotateMatrix(float32_t4 quaternion)
{
    float32_t4x4 result;
    result[0][0] = quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z;
    result[0][1] = 2.0f * (quaternion.x * quaternion.y + quaternion.w * quaternion.z);
    result[0][2] = 2.0f * (quaternion.x * quaternion.z - quaternion.w * quaternion.y);
    result[0][3] = 0.0f;
    result[1][0] = 2.0f * (quaternion.x * quaternion.y - quaternion.w * quaternion.z);
    result[1][1] = quaternion.w * quaternion.w - quaternion.x * quaternion.x + quaternion.y * quaternion.y - quaternion.z * quaternion.z;
    result[1][2] = 2.0f * (quaternion.y * quaternion.z + quaternion.w * quaternion.x);
    result[1][3] = 0.0f;
    result[2][0] = 2.0f * (quaternion.x * quaternion.z + quaternion.w * quaternion.y);
    result[2][1] = 2.0f * (quaternion.y * quaternion.z - quaternion.w * quaternion.x);
    result[2][2] = quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z;
    result[2][3] = 0.0f;
    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;
    return result;
}


float32_t4x4 MakeIdentity4x4()
{
    float32_t4x4 result;
    result[0][0] = 1.0f;
    result[0][1] = 0.0f;
    result[0][2] = 0.0f;
    result[0][3] = 0.0f;

    result[1][0] = 0.0f;
    result[1][1] = 1.0f;
    result[1][2] = 0.0f;
    result[1][3] = 0.0f;

    result[2][0] = 0.0f;
    result[2][1] = 0.0f;
    result[2][2] = 1.0f;
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;

    return result;
}


float32_t4x4 Inverse(float32_t4x4 m)
{
    float32_t4x4 result = MakeIdentity4x4();
    float determinant = m[0][0] * m[1][1] * m[2][2] * m[3][3] +
			m[0][0] * m[1][2] * m[2][3] * m[3][1] +
			m[0][0] * m[1][3] * m[2][1] * m[3][2] -

			m[0][0] * m[1][3] * m[2][2] * m[3][1] -
			m[0][0] * m[1][2] * m[2][1] * m[3][3] -
			m[0][0] * m[1][1] * m[2][3] * m[3][2] -

			m[0][1] * m[1][0] * m[2][2] * m[3][3] -
			m[0][2] * m[1][0] * m[2][3] * m[3][1] -
			m[0][3] * m[1][0] * m[2][1] * m[3][2] +

			m[0][3] * m[1][0] * m[2][2] * m[3][1] +
			m[0][2] * m[1][0] * m[2][1] * m[3][3] +
			m[0][1] * m[1][0] * m[2][3] * m[3][2] +

			m[0][1] * m[1][2] * m[2][0] * m[3][3] +
			m[0][2] * m[1][3] * m[2][0] * m[3][1] +
			m[0][3] * m[1][1] * m[2][0] * m[3][2] -

			m[0][3] * m[1][2] * m[2][0] * m[3][1] -
			m[0][2] * m[1][1] * m[2][0] * m[3][3] -
			m[0][1] * m[1][3] * m[2][0] * m[3][2] -

			m[0][1] * m[1][2] * m[2][3] * m[3][0] -
			m[0][2] * m[1][3] * m[2][1] * m[3][0] -
			m[0][3] * m[1][1] * m[2][2] * m[3][0] +

			m[0][3] * m[1][2] * m[2][1] * m[3][0] +
			m[0][2] * m[1][1] * m[2][3] * m[3][0] +
			m[0][1] * m[1][3] * m[2][2] * m[3][0];
   
    if (determinant == 0.0f)
    {
        return result;
    }
    
    float determinantRecp = 1.0f / determinant;

    result[0][0] = (m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2] - m[1][3] * m[2][2] * m[3][1] - m[1][2] * m[2][1] * m[3][3] - m[1][1] * m[2][3] * m[3][2]) * determinantRecp;
    result[0][1] = (-m[0][1] * m[2][2] * m[3][3] - m[0][2] * m[2][3] * m[3][1] - m[0][3] * m[2][1] * m[3][2] + m[0][3] * m[2][2] * m[3][1] + m[0][2] * m[2][1] * m[3][3] + m[0][1] * m[2][3] * m[3][2]) * determinantRecp;
    result[0][2] = (m[0][1] * m[1][2] * m[3][3] + m[0][2] * m[1][3] * m[3][1] + m[0][3] * m[1][1] * m[3][2] - m[0][3] * m[1][2] * m[3][1] - m[0][2] * m[1][1] * m[3][3] - m[0][1] * m[1][3] * m[3][2]) * determinantRecp;
    result[0][3] = (-m[0][1] * m[1][2] * m[2][3] - m[0][2] * m[1][3] * m[2][1] - m[0][3] * m[1][1] * m[2][2] + m[0][3] * m[1][2] * m[2][1] + m[0][2] * m[1][1] * m[2][3] + m[0][1] * m[1][3] * m[2][2]) * determinantRecp;

    result[1][0] = (-m[1][0] * m[2][2] * m[3][3] - m[1][2] * m[2][3] * m[3][0] - m[1][3] * m[2][0] * m[3][2] + m[1][3] * m[2][2] * m[3][0] + m[1][2] * m[2][0] * m[3][3] + m[1][0] * m[2][3] * m[3][2]) * determinantRecp;
    result[1][1] = (m[0][0] * m[2][2] * m[3][3] + m[0][2] * m[2][3] * m[3][0] + m[0][3] * m[2][0] * m[3][2] - m[0][3] * m[2][2] * m[3][0] - m[0][2] * m[2][0] * m[3][3] - m[0][0] * m[2][3] * m[3][2]) * determinantRecp;
    result[1][2] = (-m[0][0] * m[1][2] * m[3][3] - m[0][2] * m[1][3] * m[3][0] - m[0][3] * m[1][0] * m[3][2] + m[0][3] * m[1][2] * m[3][0] + m[0][2] * m[1][0] * m[3][3] + m[0][0] * m[1][3] * m[3][2]) * determinantRecp;
    result[1][3] = (m[0][0] * m[1][2] * m[2][3] + m[0][2] * m[1][3] * m[2][0] + m[0][3] * m[1][0] * m[2][2] - m[0][3] * m[1][2] * m[2][0] - m[0][2] * m[1][0] * m[2][3] - m[0][0] * m[1][3] * m[2][2]) * determinantRecp;

    result[2][0] = (m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1] - m[1][3] * m[2][1] * m[3][0] - m[1][1] * m[2][0] * m[3][3] - m[1][0] * m[2][3] * m[3][1]) * determinantRecp;
    result[2][1] = (-m[0][0] * m[2][1] * m[3][3] - m[0][1] * m[2][3] * m[3][0] - m[0][3] * m[2][0] * m[3][1] + m[0][3] * m[2][1] * m[3][0] + m[0][1] * m[2][0] * m[3][3] + m[0][0] * m[2][3] * m[3][1]) * determinantRecp;
    result[2][2] = (m[0][0] * m[1][1] * m[3][3] + m[0][1] * m[1][3] * m[3][0] + m[0][3] * m[1][0] * m[3][1] - m[0][3] * m[1][1] * m[3][0] - m[0][1] * m[1][0] * m[3][3] - m[0][0] * m[1][3] * m[3][1]) * determinantRecp;
    result[2][3] = (-m[0][0] * m[1][1] * m[2][3] - m[0][1] * m[1][3] * m[2][0] - m[0][3] * m[1][0] * m[2][1] + m[0][3] * m[1][1] * m[2][0] + m[0][1] * m[1][0] * m[2][3] + m[0][0] * m[1][3] * m[2][1]) * determinantRecp;

    result[3][0] = (-m[1][0] * m[2][1] * m[3][2] - m[1][1] * m[2][2] * m[3][0] - m[1][2] * m[2][0] * m[3][1] + m[1][2] * m[2][1] * m[3][0] + m[1][1] * m[2][0] * m[3][2] + m[1][0] * m[2][2] * m[3][1]) * determinantRecp;
    result[3][1] = (m[0][0] * m[2][1] * m[3][2] + m[0][1] * m[2][2] * m[3][0] + m[0][2] * m[2][0] * m[3][1] - m[0][2] * m[2][1] * m[3][0] - m[0][1] * m[2][0] * m[3][2] - m[0][0] * m[2][2] * m[3][1]) * determinantRecp;
    result[3][2] = (-m[0][0] * m[1][1] * m[3][2] - m[0][1] * m[1][2] * m[3][0] - m[0][2] * m[1][0] * m[3][1] + m[0][2] * m[1][1] * m[3][0] + m[0][1] * m[1][0] * m[3][2] + m[0][0] * m[1][2] * m[3][1]) * determinantRecp;
    result[3][3] = (m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] - m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1]) * determinantRecp;

    return result;
}


float32_t4x4 Transpose(float32_t4x4 m)
{
    float32_t4x4 result;
    result[0][0] = m[0][0];
    result[0][1] = m[1][0];
    result[0][2] = m[2][0];
    result[0][3] = m[3][0];

    result[1][0] = m[0][1];
    result[1][1] = m[1][1];
    result[1][2] = m[2][1];
    result[1][3] = m[3][1];

    result[2][0] = m[0][2];
    result[2][1] = m[1][2];
    result[2][2] = m[2][2];
    result[2][3] = m[3][2];

    result[3][0] = m[0][3];
    result[3][1] = m[1][3];
    result[3][2] = m[2][3];
    result[3][3] = m[3][3];

    return result;
}


bool AreMatricesEqual(float4x4 matrixA, float4x4 matrixB)
{
    for (int i = 0; i < 4; i++)
    {
        if (any(matrixA[i] != matrixB[i]))
        {
            return false;
        }
    }
    return true;
}


bool AreQuaternionEqual(float32_t4 quaternionA, float32_t4 quaternionB)
{
    for (int i = 0; i < 4; i++)
    {
        if (any(quaternionA != quaternionB))
        {
            return false;
        }
    }
    return true;
}


float32_t4 GetRotation(float32_t4x4 m)
{
    float32_t elem[4];
    elem[0] = m[0][0] - m[1][1] - m[2][2] + 1.0f;
    elem[1] = -m[0][0] + m[1][1] - m[2][2] + 1.0f;
    elem[2] = -m[0][0] - m[1][1] + m[2][2] + 1.0f;
    elem[3] = m[0][0] + m[1][1] + m[2][2] + 1.0f;

    int32_t biggestIdx = 0;
    for (int32_t i = 0; i < 4; i++)
    {
        if (elem[i] > elem[biggestIdx])
        {
            biggestIdx = i;
        }
    }

    if (elem[biggestIdx] < 0)
    {
        return float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    float32_t q[4];
    float32_t v = sqrt(elem[biggestIdx]) * 0.5f;
    q[biggestIdx] = v;
    float32_t mult = 0.25f / v;

    switch (biggestIdx)
    {
        case 0:
            q[1] = (m[1][0] + m[0][1]) * mult;
            q[2] = (m[0][2] + m[2][0]) * mult;
            q[3] = (m[2][1] - m[1][2]) * mult;
            break;
        case 1:
            q[0] = (m[1][0] + m[0][1]) * mult;
            q[2] = (m[2][1] + m[1][2]) * mult;
            q[3] = (m[0][2] - m[2][0]) * mult;
            break;
        case 2:
            q[0] = (m[0][2] + m[2][0]) * mult;
            q[1] = (m[2][1] + m[1][2]) * mult;
            q[3] = (m[1][0] - m[0][1]) * mult;
            break;
        case 3:
            q[0] = (m[2][1] - m[1][2]) * mult;
            q[1] = (m[0][2] - m[2][0]) * mult;
            q[2] = (m[1][0] - m[0][1]) * mult;
            break;
    }

    return float32_t4(q[0], q[1], q[2], q[3]);
}

float32_t4 LookAt(float32_t3 position, float32_t3 target)
{
    float32_t3 z = normalize(target - position);
    float32_t3 x = normalize(cross(float32_t3(0.0f, 1.0f, 0.0f), z));
    float32_t3 y = normalize(cross(z, x));

    float32_t4x4 m = MakeIdentity4x4();
    m[0][0] = x.x;
    m[0][1] = y.x;
    m[0][2] = z.x;
    m[1][0] = x.y;
    m[1][1] = y.y;
    m[1][2] = z.y;
    m[2][0] = x.z;
    m[2][1] = y.z;
    m[2][2] = z.z;

    float32_t4 rot = GetRotation(m);

    return rot;
}
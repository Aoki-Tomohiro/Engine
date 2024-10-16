struct DirectionLight
{
    float32_t4 color;
    float32_t3 direction;
    float32_t intensity;
    int32_t isEnable;
};

struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float32_t intensity;
    float32_t radius;
    float32_t decay;
    int32_t isEnable;
};

struct SpotLight
{
    float32_t4 color;
    float32_t3 position;
    float32_t intensity;
    float32_t3 direction;
    float32_t distance;
    float32_t decay;
    float32_t cosFalloffStart;
    float32_t cosAngle;
    int32_t isEnable;
};

static const int kNumDirectionalLight = 1;
static const int kNumPointLight = 1;
static const int kNumSpotLight = 1;
struct LightGroup
{
    DirectionLight directionalLights[kNumDirectionalLight];
    PointLight pointLights[kNumPointLight];
    SpotLight spotLights[kNumSpotLight];
};
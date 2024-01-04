struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct Bloom
{
    bool enable;
    float intensity;
    float highIntensityColorWeight;
    float highIntensityBlurColorWeight;
    float highIntensityShrinkBlurColorWeight;
};

struct LensDistortion
{
    bool enable;
    float tightness;
    float strength;
};

struct Fog
{
    bool enable;
    float scale;
    float attenuationRate;
};

struct DoF
{
    bool enable;
};


struct Vignette
{
    bool enable;
    float intensity;
};
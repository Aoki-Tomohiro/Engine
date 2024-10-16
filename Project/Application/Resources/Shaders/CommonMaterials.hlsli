struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    int32_t enableLighting;
    int32_t diffuseReflectionType;
    int32_t specularReflectionType;
    float32_t shininess;
    float32_t3 specularColor;
    float32_t environmentCoefficient;
    float32_t dissolveThreshold;
    float32_t edgeWidth;
    float32_t3 edgeColor;
    int32_t receiveShadows;
};
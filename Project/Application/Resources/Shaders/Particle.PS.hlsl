#include "Particle.hlsli"
#include "CommonMaterials.hlsli"
#include "LightStructs.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<LightGroup> gLightGroup : register(b1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 transformUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformUV.xy);
    float32_t4 finalColor = { 0, 0, 0, 1 };
    
    //textureのa値が0のときにPixelを棄却
    if (textureColor.a == 0.0f)
    {
        discard;
    }
    
    if (gMaterial.enableLighting != 0)
    {
        //DirectionalLight
        for (int32_t i = 0; i < kNumDirectionalLight; ++i)
        {
            if (gLightGroup.directionalLights[i].isEnable)
            {
                float32_t cos = 0.0f;
                if (gMaterial.diffuseReflectionType == 0)
                {
                    cos = saturate(dot(normalize(input.normal), -gLightGroup.directionalLights[i].direction));
                }
                else if (gMaterial.diffuseReflectionType == 1)
                {
                    float32_t NdotL = dot(normalize(input.normal), -gLightGroup.directionalLights[i].direction);
                    cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                }
        
                float32_t specularPow = 0.0f;
                if (gMaterial.specularReflectionType == 0)
                {
                    float32_t3 toEye = input.toEye;
                    float32_t3 reflectLight = reflect(gLightGroup.directionalLights[i].direction, normalize(input.normal));
                    float32_t RdotE = dot(reflectLight, toEye);
                    specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
                }
                else if (gMaterial.specularReflectionType == 1)
                {
                    float32_t3 toEye = input.toEye;
                    float32_t3 reflectLight = reflect(gLightGroup.directionalLights[i].direction, normalize(input.normal));
                    float32_t3 halfVector = normalize(-gLightGroup.directionalLights[i].direction + toEye);
                    float32_t NDotH = dot(normalize(input.normal), halfVector);
                    specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
                }
        
                //拡散反射
                float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * input.color.rgb * gLightGroup.directionalLights[i].color.rgb * cos * gLightGroup.directionalLights[i].intensity;
                //鏡面反射
                float32_t3 specular = gLightGroup.directionalLights[i].color.rgb * gLightGroup.directionalLights[i].intensity * specularPow * gMaterial.specularColor;
                //すべて加算
                finalColor.rgb += diffuse + specular;
            }
        }

        //PointLight
        for (i = 0; i < kNumPointLight; ++i)
        {
            if (gLightGroup.pointLights[i].isEnable)
            {
                float32_t3 pointLightDirection = normalize(input.worldPosition - gLightGroup.pointLights[i].position);
                float32_t distance = length(gLightGroup.pointLights[i].position - input.worldPosition); //ポイントライトへの距離
                float32_t factor = pow(saturate(-distance / gLightGroup.pointLights[i].radius + 1.0f), gLightGroup.pointLights[i].decay); //指数によるコントロール
                float32_t3 pointLightColor = gLightGroup.pointLights[i].color.rgb * gLightGroup.pointLights[i].intensity * factor;
            
                float32_t cos = 0.0f;
                if (gMaterial.diffuseReflectionType == 0)
                {
                    cos = saturate(dot(normalize(input.normal), -pointLightDirection));
                }
                else if (gMaterial.diffuseReflectionType == 1)
                {
                    float32_t NdotL = dot(normalize(input.normal), -pointLightDirection);
                    cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                }
        
                float32_t specularPow = 0.0f;
                if (gMaterial.specularReflectionType == 0)
                {
                    float32_t3 toEye = input.toEye;
                    float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
                    float32_t RdotE = dot(reflectLight, toEye);
                    specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
                }
                else if (gMaterial.specularReflectionType == 1)
                {
                    float32_t3 toEye = input.toEye;
                    float32_t3 reflectLight = reflect(pointLightDirection, normalize(input.normal));
                    float32_t3 halfVector = normalize(-pointLightDirection + toEye);
                    float32_t NDotH = dot(normalize(input.normal), halfVector);
                    specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
                }
 
                //拡散反射
                float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * input.color.rgb * pointLightColor.rgb * cos;
                //鏡面反射
                float32_t3 specular = pointLightColor.rgb * specularPow * gMaterial.specularColor;
                //すべて加算
                finalColor.rgb += diffuse + specular;
            }
        }
        
        //SpotLight
        for (i = 0; i < kNumSpotLight; ++i)
        {
            if (gLightGroup.spotLights[i].isEnable)
            {
                float32_t3 spotLightDirectionOnSurface = normalize(input.worldPosition - gLightGroup.spotLights[i].position);
                float32_t cosAngle = dot(spotLightDirectionOnSurface, gLightGroup.spotLights[i].direction);
                float32_t falloffFactor = saturate((cosAngle - gLightGroup.spotLights[i].cosAngle) / (gLightGroup.spotLights[i].cosFalloffStart - gLightGroup.spotLights[i].cosAngle));
                float32_t distance = length(gLightGroup.pointLights[i].position - input.worldPosition);
                float32_t attenuationFactor = pow(saturate(-distance / gLightGroup.spotLights[i].distance + 1.0f), gLightGroup.spotLights[i].decay);
                float32_t3 spotLightColor = gLightGroup.spotLights[i].color.rgb * gLightGroup.spotLights[i].intensity * attenuationFactor * falloffFactor;
            
                float32_t cos = 0.0f;
                if (gMaterial.diffuseReflectionType == 0)
                {
                    cos = saturate(dot(normalize(input.normal), -spotLightDirectionOnSurface));
                }
                else if (gMaterial.diffuseReflectionType == 1)
                {
                    float32_t NdotL = dot(normalize(input.normal), -spotLightDirectionOnSurface);
                    cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
                }
        
                float32_t specularPow = 0.0f;
                if (gMaterial.specularReflectionType == 0)
                {
                    float32_t3 toEye = input.toEye;
                    float32_t3 reflectLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
                    float32_t RdotE = dot(reflectLight, toEye);
                    specularPow = pow(saturate(RdotE), gMaterial.shininess); //反射強度
                }
                else if (gMaterial.specularReflectionType == 1)
                {
                    float32_t3 toEye = input.toEye;
                    float32_t3 reflectLight = reflect(spotLightDirectionOnSurface, normalize(input.normal));
                    float32_t3 halfVector = normalize(-spotLightDirectionOnSurface + toEye);
                    float32_t NDotH = dot(normalize(input.normal), halfVector);
                    specularPow = pow(saturate(NDotH), gMaterial.shininess); //反射強度
                }
            
                //拡散反射
                float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * input.color.rgb * spotLightColor.rgb * cos;
                //鏡面反射
                float32_t3 specular = spotLightColor.rgb * specularPow * gMaterial.specularColor;
                //すべて加算
                finalColor.rgb += diffuse + specular;
            }
        }
    }
    else
    {
        finalColor.rgb = gMaterial.color.rgb * textureColor.rgb * input.color.rgb;
    }
    
    //最終的な色を決定
    output.color.rgb = finalColor.rgb;
    //アルファは今まで道り
    output.color.a = gMaterial.color.a * textureColor.a * input.color.a;
    
    return output;
}
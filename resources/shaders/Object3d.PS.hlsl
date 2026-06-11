#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
    int32_t lightingModel; // 0: Phong, 1: Blinn-Phong, 2:PointLight, 3:SpotLight
};

struct DirectionalLight
{
    float32_t4 color; //ライトの色
    float32_t3 direction; //ライトの向き
    float intensity; //輝度
};

struct Camera
{
    float32_t3 worldPosition;
};

struct PointLight
{
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの位置
    float intensity; //輝度
    float radius; // ライトの届く最大距離
    float decay; // 減衰率
};

struct SpotLight
{
    float32_t4 color; //!<ライトの色
    float32_t3 position; //ライトの位置
    float32_t intensity; //輝度
    float32_t3 direction; //スポットライトの方向
    float32_t distance; //ライトの届く最大距離
    float32_t decay; //減衰率
    float32_t cosAngle; //スポットライトの余弦
    float32_t cosFalloffStart; //減衰開始角の余弦
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3);
ConstantBuffer<SpotLight> gSpotLight : register(b4);

//TextureCube<float32_t4> gEnvironmentTexture : register(t1);

struct LightingResult
{
    float32_t3 diffuse;
    float32_t3 specular;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};


LightingResult CreateDirectionalLight(DirectionalLight light, float32_t3 normal, float32_t3 toEye, float32_t4 baseColor, float32_t shininess)
{

    LightingResult result;
    
    float NdotL = dot(
    (normal), -light.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    
      //Phong + Half-Lambert
    float32_t3 reflectLight = reflect(-light.direction, normalize(normal)); //r
    float RdotE = dot(reflectLight, toEye);
    float specularPow = pow(saturate(RdotE), shininess); //反射強度
        
          //拡散反射
    result.diffuse = baseColor.rgb * light.color.rgb * cos * light.intensity;
        
    //鏡面反射
    result.specular =
   light.color.rgb * light.intensity * specularPow;
    
    return result;
    
    
}

LightingResult CreateDirectionalLightBlinnPhong(DirectionalLight light, float32_t3 normal, float32_t3 toEye, float32_t4 baseColor, float32_t shininess)
{
    
    LightingResult result;

    float NdotL = dot(normalize(normal), -light.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);

   //Blinn-Phong
    float32_t3 halfVector = normalize(-light.direction + toEye);
    float NDotH = dot(normalize(normal), halfVector);
    float specularPow = pow(saturate(NDotH), shininess);

   
         //拡散反射
    result.diffuse = baseColor.rgb * light.color.rgb * cos * light.intensity;
        
    //鏡面反射
    result.specular =
    light.color.rgb * light.intensity * specularPow;

    return result;
    
}

LightingResult CreatePointLight(PointLight light, float32_t3 normal, float32_t3 worldPosition, float32_t3 toEye, float32_t4 baseColor, float32_t shininess)
{
    
    LightingResult result;
    
    //PointLight
    float32_t3 lightDirection = normalize(worldPosition - light.position);
       
        
        //鏡面
    float32_t3 reflectLight = reflect(-lightDirection, normalize(normal)); //r
    float RdotE = dot(reflectLight, toEye);
    float specularPow = pow(saturate(RdotE), shininess); //反射強度
         //拡散
    float NDotL = dot(normalize(normal), -lightDirection);
    float cos = pow(NDotL * 0.5f + 0.5f, 2.0f);
    
    
     //逆二乗則による減衰係数の計算
    float32_t distance = length(light.position - worldPosition); //ポイントライトへの距離
    float32_t factor = pow(saturate(-distance / light.radius + 1.0f), light.decay); //指数によるコントロール
        
        
    //拡散反射
    result.diffuse =
    baseColor.rgb * light.color.rgb * cos * light.intensity * factor;
  
        
    //鏡面反射
    result.specular =
    light.color.rgb * light.intensity * specularPow * factor;
    
    
    return result;
    
}

LightingResult CreateSpotLight(SpotLight light, float32_t3 normal, float32_t3 worldPosition, float32_t3 toEye, float32_t4 baseColor, float32_t shininess)
{
    
    LightingResult result;
    
     //SpotLight
    float32_t3 lightDirection = normalize(light.position - worldPosition);
        
    //鏡面
    float32_t3 reflectLight = reflect(-lightDirection, normalize(normal)); //r
    float RdotE = dot(reflectLight, toEye);
    float specularPow = pow(saturate(RdotE), shininess); //反射強度
    //拡散
    float NDotL = dot(normalize(normal), lightDirection); //n・l
    float cos = pow(NDotL * 0.5f + 0.5f, 2.0f);
        
    //距離による減衰係数の計算
    float32_t distance = length(light.position - worldPosition); //ポイントライトへの距離
    float32_t factor = pow(saturate(-distance / light.distance + 1.0f), light.decay); //指数によるコントロール
        
        
    //角度減衰
    float32_t3 DirectionOnSurface = normalize(worldPosition - light.position);
    float32_t cosAngle = dot(-DirectionOnSurface, light.direction);
    float32_t falloffFactor = saturate((cosAngle - light.cosAngle) / (light.cosFalloffStart - light.cosAngle));
        
        
    //拡散反射
    result.diffuse =
    baseColor.rgb * light.color.rgb * cos * light.intensity * factor * falloffFactor;
        
    //鏡面反射
    result.specular =
    light.color.rgb * light.intensity * specularPow * factor * falloffFactor;
    
    return result;
    
}


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float32_t4(input.texcoord,0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    if (textureColor.a <= 0.5)
    {
        discard;
    }
    
    if (gMaterial.enableLighting != 0)//Lightingする場合
    {
       
       
       
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition); //v
        
        float32_t4 baseColor = gMaterial.color * textureColor;
        
        if (gMaterial.lightingModel == 0)
        {
        
            LightingResult directional =
            CreateDirectionalLight(gDirectionalLight, input.normal, toEye, baseColor, gMaterial.shininess);
            output.color.rgb = directional.diffuse + directional.specular;
            
        }
        else if (gMaterial.lightingModel == 1)
        {
             
            LightingResult directional =
            CreateDirectionalLightBlinnPhong(gDirectionalLight, input.normal, toEye, baseColor, gMaterial.shininess);
            output.color.rgb = directional.diffuse + directional.specular;
        }
        else if (gMaterial.lightingModel == 2)
        {
            LightingResult directional = CreateDirectionalLight(gDirectionalLight, input.normal, toEye, baseColor, gMaterial.shininess);

            LightingResult pointLight = CreatePointLight(gPointLight, input.normal, input.worldPosition, toEye, baseColor, gMaterial.shininess);

            output.color.rgb = (directional.diffuse + directional.specular) + (pointLight.diffuse + pointLight.specular);
        }
        else if (gMaterial.lightingModel == 3)
        {
            
            LightingResult directional = CreateDirectionalLight(gDirectionalLight, input.normal, toEye, baseColor, gMaterial.shininess);
            
            LightingResult spot = CreateSpotLight(gSpotLight, input.normal, input.worldPosition, toEye, baseColor, gMaterial.shininess);
            
            output.color.rgb = (directional.diffuse + directional.specular) + (spot.diffuse + spot.specular);
          
            
        }
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else //Lightingしない場合。前回までと同じ演算
    {
        output.color = gMaterial.color * textureColor;
    }
    return output;
}
#version 330

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;
const int MAX_TEXTURES_NUM = 2;

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;
in vec4 LightSpacePos;

out vec4 FragColor;

struct BaseLight {
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight {
    BaseLight Base;
    vec3 Direction;
};

struct Attenuation {
    float Constant;
    float Linear;
    float Exp;
};

struct PointLight {
    BaseLight Base;
    vec3 Position;
    Attenuation Atten;
};

struct SpotLight {
    PointLight Base;
    vec3 Direction;
    float Cutoff;
};

struct TextureType{
    sampler2D Sampler;
    float Intensity;
};

uniform int gNumPointLights;
uniform int gNumSpotLights;
uniform DirectionalLight gDirectionalLight;
uniform PointLight gPointLights[MAX_POINT_LIGHTS];
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];
uniform sampler2D gShadowMap;

uniform TextureType gTextures[MAX_TEXTURES_NUM];

uniform vec3 gEyeWorldPos;
uniform float gMatSpecularIntensity;
uniform float gSpecularPower;

float CalcShadowFactor(vec4 LightSpacePos)
{
    vec3 ProjCoords = LightSpacePos.xyz / LightSpacePos.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    float Depth = texture(gShadowMap, UVCoords).x;
    if (Depth < (z + 0.00001)) return 0.5;       // +0.00001 to avoid error of float operation
    else return 1.0;
}

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal, float ShadowFactor) {
    // 环境光=颜色*强度
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);

    // 漫反射因子
    float DiffuseFactor = dot(Normal, -LightDirection);

  // 有反射条件下：漫反射+镜面反射，套公式
    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);
    if(DiffuseFactor > 0) {
        // 漫反射色
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);

        // 镜面反射因子
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        if(SpecularFactor > 0) {    
            // 镜面反射色                                               
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(Light.Color, 1.0f) * gMatSpecularIntensity * SpecularFactor;
        }
    }
    //return (AmbientColor + DiffuseColor + SpecularColor);
    return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor));
}

vec4 CalcPointLight(PointLight l, vec3 Normal, vec4 LightSpacePos) {
    vec3 LightDirection = WorldPos0 - l.Position;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    float ShadowFactor = CalcShadowFactor(LightSpacePos);

    vec4 Color = CalcLightInternal(l.Base, LightDirection, Normal, ShadowFactor);
    float AttenuationFactor = l.Atten.Constant +
        l.Atten.Linear * Distance +
        l.Atten.Exp * Distance * Distance;

    return Color / AttenuationFactor;
}

vec4 CalcDirectionalLight(vec3 Normal) {
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal, 1.0);
}

vec4 CalcSpotLight(SpotLight l, vec3 Normal, vec4 LightSpacePos) {
    vec3 LightToPixel = normalize(WorldPos0 - l.Base.Position);
    float SpotFactor = dot(LightToPixel, l.Direction);

    if(SpotFactor > l.Cutoff) {
        vec4 Color = CalcPointLight(l.Base, Normal, LightSpacePos);
        // CalcPointLight(l.Base, Normal);
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0 / (1.0 - l.Cutoff));
    } else {
        return vec4(0, 0, 0, 0);
    }
}

void main() {
    vec3 Normal = normalize(Normal0);
    vec4 TotalLight = CalcDirectionalLight(Normal);

    for(int i = 0; i < gNumPointLights; i++) {
        TotalLight += CalcPointLight(gPointLights[i], Normal, LightSpacePos);
    }

    for(int i = 0; i < gNumSpotLights; i++) {
        TotalLight += CalcSpotLight(gSpotLights[i], Normal, LightSpacePos);
    }

    vec4 texture = vec4(0.0, 0.0, 0.0, 0.0);
    for(int i = 0; i < MAX_TEXTURES_NUM; i++) {
        texture = mix(texture, texture2D(gTextures[i].Sampler, TexCoord0.xy), gTextures[i].Intensity);
    }

    FragColor = texture * TotalLight;
}

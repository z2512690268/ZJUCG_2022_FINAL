#version 330

in vec2 TexCoord0;
in vec3 Normal0;
in vec3 WorldPos0;  

out vec4 FragColor;

struct DirectionalLight {
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};

uniform DirectionalLight gDirectionalLight;
uniform sampler2D gSampler;                                                         
uniform vec3 gEyeWorldPos;                                                          
uniform float gMatSpecularIntensity;                                                
uniform float gSpecularPower;    

void main() {
    // 环境光=颜色*强度
    vec4 AmbientColor = vec4(gDirectionalLight.Color *
        gDirectionalLight.AmbientIntensity, 1.0f);
    
    // 漫反射因子
    vec3 LightDirection = -gDirectionalLight.Direction;                             
    vec3 Normal = normalize(Normal0);  
    float DiffuseFactor = dot(Normal, LightDirection);

    // 有反射条件下：漫反射+镜面反射，套公式
    vec4 DiffuseColor = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);
    if(DiffuseFactor > 0) {
        // 漫反射色
        DiffuseColor = vec4(
        gDirectionalLight.Color * 
            gDirectionalLight.DiffuseIntensity * DiffuseFactor,
         1.0f);
        
        // 镜面反射因子
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                     
        vec3 LightReflect = normalize(reflect(gDirectionalLight.Direction, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);      
        if (SpecularFactor > 0) {    
            // 镜面反射色                                               
            SpecularFactor = pow(SpecularFactor, gSpecularPower);
            SpecularColor = vec4(gDirectionalLight.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
        }      
    }
    // 直接光照色+散射光照色
    FragColor = texture2D(gSampler, TexCoord0.xy) *
        (AmbientColor + DiffuseColor  + SpecularColor);
}

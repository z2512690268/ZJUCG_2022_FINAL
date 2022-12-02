#version 330

in vec2 TexCoord0;
in vec3 Normal0;

out vec4 FragColor;

struct DirectionalLight {
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
    vec3 Direction;
};

uniform DirectionalLight gDirectionalLight;
uniform sampler2D gSampler;

void main() {
    // 颜色*强度
    vec4 AmbientColor = vec4(gDirectionalLight.Color, 1.0f) *
        gDirectionalLight.AmbientIntensity;
    // cos法线夹角
    float DiffuseFactor = dot(normalize(Normal0), -gDirectionalLight.Direction);

    // 有反射条件下：颜色*反射强度*cos法线夹角
    vec4 DiffuseColor;

    if(DiffuseFactor > 0) {
        DiffuseColor = vec4(gDirectionalLight.Color, 1.0f) *
            gDirectionalLight.DiffuseIntensity *
            DiffuseFactor;
    } else {
        DiffuseColor = vec4(0, 0, 0, 0);
    }
    // 直接光照色+散射光照色
    FragColor = texture2D(gSampler, TexCoord0.xy) *
        (AmbientColor + DiffuseColor);
}

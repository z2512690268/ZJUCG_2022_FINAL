#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 Normal;
layout(location = 3) in mat4 WVP;
layout(location = 7) in mat4 World;
                                                                  
uniform mat4 gLightWVP;                                                             

out vec4 LightSpacePos;
out vec2 TexCoord0;
out vec3 Normal0;
out vec3 WorldPos0;

void main() {
    gl_Position = WVP * vec4(Position, 1.0);
    LightSpacePos = gLightWVP * vec4(Position, 1.0);
    TexCoord0 = TexCoord;
    // 将法线向量转换到世界空间
    Normal0 = (World * vec4(Normal, 0.0)).xyz;
    WorldPos0 = (World * vec4(Position, 1.0)).xyz;
}

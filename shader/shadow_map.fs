#version 330

in vec2 TexCoordOut;
uniform sampler2D gShadowMap;

out vec4 FragColor;

void main()
{
  //float Depth = texture(gShadowMap, TexCoordOut).x;
  float Depth = 1.0 - (1.0 - texture(gShadowMap, TexCoordOut).x) * 25.0;
  FragColor = vec4(Depth);
}
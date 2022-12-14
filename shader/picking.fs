#version 330

uniform uint gObjectIndex;

out vec3 FragColor;

void main()
{
    FragColor = vec3(float(gObjectIndex), 0.0f, float(gl_PrimitiveID+1));
}
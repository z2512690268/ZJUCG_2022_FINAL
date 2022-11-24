#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <string>

#include <GL/glew.h>

class Texture
{
public:
    Texture(GLenum TextureTarget, const std::string& FileName);

    Texture(GLenum TextureTarget);

    // Should be called once to load the texture
    bool Load();

    void Load(unsigned int BufferSize, void* pData);

    void Load(const std::string& Filename);

    void LoadRaw(int Width, int Height, int BPP, unsigned char* pData);

    // Must be called at least once for the specific texture unit
    void Bind(GLenum TextureUnit);

    void GetImageSize(int& ImageWidth, int& ImageHeight)
    {
        ImageWidth = m_imageWidth;
        ImageHeight = m_imageHeight;
    }

private:
    void LoadInternal(void* image_data);

    std::string m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    int m_imageBPP = 0;
};


#endif
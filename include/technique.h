#ifndef TECHNIQUE_H
#define TECHNIQUE_H

#include <list>
#include <GL/glew.h>
#include "file.h"

class Technique
{
public:

    Technique();

    virtual ~Technique();

    virtual bool Init();

    void Enable();

    void Disable();

    GLuint GetProgram() const { return m_shaderProg; }

protected:

    bool AddShader(GLenum ShaderType, const char* pFilename);

    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);

    GLuint m_shaderProg = 0;

private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
    FileManager fileManager;
};

#endif  /* TECHNIQUE_H */

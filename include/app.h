#ifndef _APP_H
#define _APP_H

#include "callback.h"

class App : public ICallbacks
{
public:
    virtual void Run() {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glFrontFace(GL_CW);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        RegisterICallback(this);
        InitCallbacks();
        glutMainLoop();   
    }
private:
};

#endif
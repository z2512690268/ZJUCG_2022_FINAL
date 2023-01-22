#ifndef _SCENECTRL_H
#define _SCENECTRL_H

#include "scene.h"
#include <vector>

class SceneController
{
public:
    SceneController() {
        m_curScene = 1;
        m_scenes.push_back(nullptr);
    }

    int AddScene(Scene* pScene) {
        m_scenes.push_back(pScene);
        return m_scenes.size() - 1;
    }

    void RemoveScene(int Index) {
        if (Index < 0 || Index >= m_scenes.size()) {
            return;
        }
        m_scenes[Index] = nullptr;
    }

    bool Run(int argc, char* argv[]) {
        while(m_curScene >= 0 && m_curScene < m_scenes.size()) {
            Scene* pScene = m_scenes[m_curScene];
        
            pScene->Preinit(argc, argv);
            pScene->Init();
            pScene->Postinit(argc, argv);
            int ret = pScene->Run();
            pScene->Preback(argc, argv);
            pScene->Back();
            pScene->Postback(argc, argv);

            if(!ret)    break;
            else        m_curScene = ret;
        }
        return true;
    }
    
private:
    std::vector<Scene*> m_scenes;
    int m_curScene;
};

#endif
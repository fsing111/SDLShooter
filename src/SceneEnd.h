#ifndef SCENE_END_H
#define SCENE_END_H

#include "Scene.h"
#include <string>
#include <SDL_mixer.h>

class SceneEnd : public Scene{
public:
    virtual void init();
    virtual void update(float deltaTime);
    virtual void render();
    virtual void clean();
    virtual void handleEvent(SDL_Event* event);

private:
    bool isTyping = true;
    std::string name = "";

    void renderPhase1();
    void renderPhase2();

    void removeLastUTF8Char(std::string& str);

    float blinkTimer = 1.0f;  //光标闪烁计时器
    Mix_Music* bgm; //背景音乐
};

#endif // SCENE_END_H

#ifndef SCENE_SETTINGS_H
#define SCENE_SETTINGS_H

#include "Scene.h"
#include <SDL_mixer.h>
#include <string>
#include <vector>

class SceneSettings : public Scene
{
public:
    virtual void init() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event* event) override;

private:
    struct MenuItem {
        std::string label;
        int value;       // 当前值
        int minValue;    // 最小值
        int maxValue;    // 最大值
        int step;        // 步长
    };

    std::vector<MenuItem> menuItems;
    int selectedIndex = 0;
    float blinkTimer = 0.0f; // 光标闪烁计时器

    void setupMenuItems();
    void applySettings();
    void renderProgressBar(int x, int y, int width, int height, float progress);
};

#endif // SCENE_SETTINGS_H

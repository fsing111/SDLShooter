#include "SceneSettings.h"
#include "Game.h"

void SceneSettings::init()
{
    setupMenuItems();
    selectedIndex = 0;
    blinkTimer = 0.0f;
}

void SceneSettings::setupMenuItems()
{
    menuItems.clear();

    // 音乐音量 (0-128)
    MenuItem musicVol;
    musicVol.label = "音乐音量";
    musicVol.minValue = 0;
    musicVol.maxValue = 128;
    musicVol.step = 8;
    musicVol.value = Mix_VolumeMusic(-1); // 读取当前音量
    menuItems.push_back(musicVol);

    // 音效音量 (0-128)
    MenuItem sfxVol;
    sfxVol.label = "音效音量";
    sfxVol.minValue = 0;
    sfxVol.maxValue = 128;
    sfxVol.step = 8;
    sfxVol.value = Mix_Volume(-1, -1); // 读取当前音效音量
    menuItems.push_back(sfxVol);

    // 全屏模式 (0=窗口, 1=全屏)
    MenuItem fullscreen;
    fullscreen.label = "全屏模式";
    fullscreen.minValue = 0;
    fullscreen.maxValue = 1;
    fullscreen.step = 1;
    fullscreen.value = game.isFullscreen() ? 1 : 0;
    menuItems.push_back(fullscreen);
}

void SceneSettings::applySettings()
{
    if (menuItems.size() < 3) return;

    // 应用音乐音量
    Mix_VolumeMusic(menuItems[0].value);

    // 应用音效音量
    Mix_Volume(-1, menuItems[1].value);

    // 应用全屏设置
    bool wantFullscreen = (menuItems[2].value == 1);
    if (wantFullscreen != game.isFullscreen()) {
        game.setFullscreen(wantFullscreen);
        if (wantFullscreen) {
            SDL_SetWindowFullscreen(game.getWindow(), SDL_WINDOW_FULLSCREEN);
        } else {
            SDL_SetWindowFullscreen(game.getWindow(), 0);
        }
    }
}

void SceneSettings::update(float deltaTime)
{
    blinkTimer += deltaTime;
    if (blinkTimer > 1.0f) {
        blinkTimer -= 1.0f;
    }
}

void SceneSettings::render()
{
    // 绘制半透明黑色遮罩
    SDL_SetRenderDrawBlendMode(game.getRenderer(), SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(game.getRenderer(), 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, game.getWindowWidth(), game.getWindowHeight()};
    SDL_RenderFillRect(game.getRenderer(), &overlay);

    // 绘制设置面板背景
    int panelWidth = 440;
    int panelHeight = 380;
    int panelX = (game.getWindowWidth() - panelWidth) / 2;
    int panelY = (game.getWindowHeight() - panelHeight) / 2;

    SDL_SetRenderDrawColor(game.getRenderer(), 20, 20, 40, 230);
    SDL_Rect panelBg = {panelX, panelY, panelWidth, panelHeight};
    SDL_RenderFillRect(game.getRenderer(), &panelBg);

    // 绘制面板边框
    SDL_SetRenderDrawColor(game.getRenderer(), 100, 150, 255, 255);
    SDL_RenderDrawRect(game.getRenderer(), &panelBg);

    // 标题
    game.renderTextCentered("设  置", 0.15, true);

    // 菜单项
    int startY = panelY + 80;
    int itemHeight = 70;

    for (int i = 0; i < (int)menuItems.size(); i++) {
        int y = startY + i * itemHeight;
        bool isSelected = (i == selectedIndex);

        // 选中项高亮背景
        if (isSelected) {
            SDL_SetRenderDrawColor(game.getRenderer(), 40, 50, 80, 200);
            SDL_Rect highlight = {panelX + 10, y - 5, panelWidth - 20, itemHeight - 10};
            SDL_RenderFillRect(game.getRenderer(), &highlight);

            // 左侧箭头指示器
            if (blinkTimer < 0.7f) {
                game.renderTextPos("▶", panelX + 20, y + 5);
            }
        }

        // 标签
        game.renderTextPos(menuItems[i].label, panelX + 55, y + 5);

        // 值显示
        int barX = panelX + 200;
        int barY = y + 10;
        int barWidth = 200;
        int barHeight = 20;

        if (menuItems[i].maxValue == 1) {
            // 布尔选项：显示 开/关
            std::string valText = (menuItems[i].value == 1) ? "开" : "关";
            game.renderTextPos(valText, barX + 70, y + 5);
        } else {
            // 数值选项：显示进度条
            float progress = (float)(menuItems[i].value - menuItems[i].minValue) /
                             (float)(menuItems[i].maxValue - menuItems[i].minValue);
            renderProgressBar(barX, barY, barWidth, barHeight, progress);
        }
    }

    // 底部提示
    if (blinkTimer < 0.7f) {
        game.renderTextCentered("ESC 返回游戏", 0.82, false);
    }
}

void SceneSettings::renderProgressBar(int x, int y, int width, int height, float progress)
{
    // 背景
    SDL_SetRenderDrawColor(game.getRenderer(), 60, 60, 80, 255);
    SDL_Rect bgRect = {x, y, width, height};
    SDL_RenderFillRect(game.getRenderer(), &bgRect);

    // 填充
    int fillWidth = (int)(width * progress);
    if (fillWidth > 0) {
        SDL_SetRenderDrawColor(game.getRenderer(), 80, 180, 255, 255);
        SDL_Rect fillRect = {x, y, fillWidth, height};
        SDL_RenderFillRect(game.getRenderer(), &fillRect);
    }

    // 边框
    SDL_SetRenderDrawColor(game.getRenderer(), 120, 160, 220, 255);
    SDL_RenderDrawRect(game.getRenderer(), &bgRect);
}

void SceneSettings::handleEvent(SDL_Event* event)
{
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                applySettings();
                game.closeSettings();
                break;
            case SDL_SCANCODE_UP:
                selectedIndex = (selectedIndex - 1 + menuItems.size()) % menuItems.size();
                break;
            case SDL_SCANCODE_DOWN:
                selectedIndex = (selectedIndex + 1) % menuItems.size();
                break;
            case SDL_SCANCODE_LEFT: {
                MenuItem& item = menuItems[selectedIndex];
                item.value = (item.value - item.step < item.minValue) ? item.minValue : item.value - item.step;
                // 实时预览音量变化
                if (selectedIndex == 0) Mix_VolumeMusic(item.value);
                if (selectedIndex == 1) Mix_Volume(-1, item.value);
                break;
            }
            case SDL_SCANCODE_RIGHT: {
                MenuItem& item = menuItems[selectedIndex];
                item.value = (item.value + item.step > item.maxValue) ? item.maxValue : item.value + item.step;
                // 实时预览音量变化
                if (selectedIndex == 0) Mix_VolumeMusic(item.value);
                if (selectedIndex == 1) Mix_Volume(-1, item.value);
                break;
            }
            case SDL_SCANCODE_RETURN:
                // 对布尔选项，回车切换
                if (menuItems[selectedIndex].maxValue == 1) {
                    menuItems[selectedIndex].value = !menuItems[selectedIndex].value;
                    // 实时预览全屏切换
                    if (selectedIndex == 2) {
                        bool wantFullscreen = (menuItems[2].value == 1);
                        game.setFullscreen(wantFullscreen);
                        if (wantFullscreen) {
                            SDL_SetWindowFullscreen(game.getWindow(), SDL_WINDOW_FULLSCREEN);
                        } else {
                            SDL_SetWindowFullscreen(game.getWindow(), 0);
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
}

void SceneSettings::clean()
{
    // 无需特殊清理
}

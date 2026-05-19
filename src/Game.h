#ifndef GAME_H
#define GAME_H

#include "Scene.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "Object.h"
#include <string>
#include <map>
#include <fstream>

// 前置声明
class SceneSettings;
class Game
{
public:
    static Game& getInstance(){
        static Game instance;
        return instance;
    }

    ~Game();
    void run();
    void init();
    void clean();
    void changeScene(Scene* scene);

    void handleEvent(SDL_Event *event);
    void update(float deltaTime);
    void render();
    
    SDL_Window* getWindow() { return window; }
    SDL_Renderer* getRenderer() { return renderer; }
    int getWindowWidth() { return windowWidth; }
    int getWindowHeight() { return windowHeight; }

    void backgroundUpdate(float deltaTime);
    void renderBackground();

    
    // setters
    void setFinalScore(int score) { finalScore = score; }

    // getters
    int getFinalScore() { return finalScore; }

    // 渲染文字函数，返回文本结束位置的坐标点
    SDL_Point renderTextCentered(std::string text, float posY, bool isTitle);
    void renderTextPos(std::string text, int posX, int posY, bool isLeft = true);

    void insertLeaderBoard(int score, std::string name); // 插入排行榜
    std::multimap<int, std::string, std::greater<int>>& getLeaderBoard() { return leaderBoard; } // 返回leaderBoard的引用

    // 设置菜单相关
    void showSettings();   // 显示设置菜单
    void closeSettings();  // 关闭设置菜单
    bool isPaused() { return paused; } // 游戏是否暂停
    bool isFullscreen() { return isFullscreenFlag; }
    void setFullscreen(bool value) { isFullscreenFlag = value; }


private:
    Game();
    // 删除拷贝与赋值构造函数
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    bool isRunning = true;
    Scene* currentScene = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;
    int FPS = 60;            // 目标帧率
    Uint32 frameTime;        // 每帧的目标时间（毫秒）
    float deltaTime;         // 两帧之间的时间差（秒）

    Background nearStars;  // 近处的星星
    Background farStars;   // 远处的星星

    TTF_Font* titleFont;
    TTF_Font* textFont;

    int finalScore = 0; // 最终得分

    std::multimap<int, std::string, std::greater<int>> leaderBoard; // 使用multimap来存储分数和名字，分数作为键，名字作为值
    void saveData(); // 保存排行榜数据到文件
    void loadData();  // 从文件中加载数据到排行榜
    bool isFullscreenFlag = false; // 是否全屏
    bool paused = false;           // 游戏是否暂停（设置菜单打开时）
    SceneSettings* settingsScene = nullptr; // 设置菜单场景
};
#endif
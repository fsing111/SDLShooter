#include "Game.h"
#include "SceneMain.h"
#include "SceneTitle.h"
#include "SceneSettings.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

Game::Game()
{
}

Game::~Game()
{
    saveData();
    clean();
}

void Game::run()
{
    while (isRunning)
    {
        auto frameStart = SDL_GetTicks();  // 记录帧开始时间
        
        SDL_Event event;
        handleEvent(&event);
        update(deltaTime);  // 传递deltaTime
        render();
        
        auto frameEnd = SDL_GetTicks();  // 记录帧结束时间
        auto diff = frameEnd - frameStart;  // 计算帧处理时间
        
        // 帧率限制和deltaTime计算
        if (diff < frameTime){
            SDL_Delay(frameTime - diff);  // 如果处理太快，延迟一下
            deltaTime = frameTime / 1000.0f;  // 转换为秒
        }
        else{
            deltaTime = diff / 1000.0f;  // 如果处理较慢，使用实际时间
        }
    }
    
}

void Game::init()
{
    frameTime = 1000 / FPS;
    // SDL 初始化
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 创建窗口
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 创建渲染器
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        isRunning = false;
    }
    // 设置逻辑分辨率
    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
    // 初始化SDL_image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
        isRunning = false;
    }

    // 初始化SDL_mixer
    if (Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        isRunning = false;
    }

    // 打开音频设备
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mixer could not open audio! SDL_mixer Error: %s\n", Mix_GetError());
        isRunning = false;
    }
    
    // 设置音效channel数量
    Mix_AllocateChannels(32);

    // 设置音乐音量
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 8);
    
    //字体的初始化
    if (TTF_Init() != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        isRunning = false;
    }

    // 载入字体
    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
    if (titleFont == nullptr || textFont == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_OpenFont: %s\n", TTF_GetError());
        isRunning = false;
    }

    currentScene = new SceneTitle();
    currentScene->init();

    // 初始化背景卷轴
    nearStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-A.png");
    SDL_QueryTexture(nearStars.texture, NULL, NULL, &nearStars.width, &nearStars.height);   
    nearStars.height /= 2;
    nearStars.width /= 2;

    farStars.texture = IMG_LoadTexture(renderer, "assets/image/Stars-B.png");
    SDL_QueryTexture(farStars.texture, NULL, NULL, &farStars.width, &farStars.height);
    farStars.height /= 2;
    farStars.width /= 2;
    farStars.speed = 20;  // 远处的星星移动速度较慢

    // 载入得分
    loadData();
}

void Game::clean()
{
    if (settingsScene != nullptr)
    {
        settingsScene->clean();
        delete settingsScene;
        settingsScene = nullptr;
    }
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // 清理SDL_mixer
    Mix_CloseAudio();
    Mix_Quit();

    if (nearStars.texture != nullptr){
        SDL_DestroyTexture(nearStars.texture);
    }
    if (farStars.texture != nullptr){
        SDL_DestroyTexture(farStars.texture);
    }
}

void Game::changeScene(Scene *scene)
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    currentScene = scene;
    currentScene->init();
}

void Game::showSettings()
{
    if (paused) return; // 已经在暂停状态
    paused = true;
    settingsScene = new SceneSettings();
    settingsScene->init();
}

void Game::closeSettings()
{
    if (!paused) return; // 不在暂停状态
    paused = false;
    if (settingsScene != nullptr) {
        settingsScene->clean();
        delete settingsScene;
        settingsScene = nullptr;
    }
}

void Game::handleEvent(SDL_Event *event)
{
    while (SDL_PollEvent(event))
    {
        if (event->type == SDL_QUIT)
        {
            isRunning = false;
        }
        if (event->type == SDL_KEYDOWN){
            // ESC键：切换设置菜单
            if (event->key.keysym.scancode == SDL_SCANCODE_ESCAPE){
                if (paused) {
                    closeSettings();
                } else {
                    showSettings();
                }
                continue; // 不再传递ESC给场景
            }
            // F4键：切换全屏（仅在非暂停时）
            if (event->key.keysym.scancode == SDL_SCANCODE_F4 && !paused){
                isFullscreenFlag = !isFullscreenFlag;
                if (isFullscreenFlag){
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                }else{
                    SDL_SetWindowFullscreen(window, 0);
                }
            }
        }

        // 暂停时，事件交给设置菜单处理；否则交给当前场景
        if (paused && settingsScene != nullptr) {
            settingsScene->handleEvent(event);
        } else {
            currentScene->handleEvent(event);
        }
    }
}


void Game::update(float deltaTime)
{
    backgroundUpdate(deltaTime);
    if (paused && settingsScene != nullptr) {
        settingsScene->update(deltaTime);
    } else {
        currentScene->update(deltaTime);    // 传递deltaTime给当前场景
    }
}

void Game::render()
{
    // 重置绘制颜色为黑色（防止设置菜单修改颜色后影响清屏）
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    // 清空
    SDL_RenderClear(renderer);

    // 渲染星空背景
    renderBackground();

    currentScene->render();

    // 暂停时渲染设置菜单（覆盖在游戏画面上方）
    if (paused && settingsScene != nullptr) {
        settingsScene->render();
    }

    // 显示更新
    SDL_RenderPresent(renderer);
}

void Game::backgroundUpdate(float deltaTime)
{
    nearStars.offset += nearStars.speed * deltaTime;
    if (nearStars.offset >= 0)
    {
        nearStars.offset -= nearStars.height;
    }

    farStars.offset += farStars.speed * deltaTime;
    if (farStars.offset >= 0){
        farStars.offset -= farStars.height;
    }
}

void Game::renderBackground()
{
    // 渲染远处的星星
    for (int posY = static_cast<int>(farStars.offset); posY < getWindowHeight(); posY += farStars.height){
        for (int posX = 0; posX < getWindowWidth(); posX += farStars.width){
            SDL_Rect ds = {posX, posY, farStars.width, farStars.height};
            SDL_RenderCopy(renderer, farStars.texture, NULL, &ds);
        }
    }
    // 渲染近处的星星
    for (int posY = static_cast<int>(nearStars.offset); posY < getWindowHeight(); posY += nearStars.height)
    {
        for (int posX = 0; posX < getWindowWidth(); posX += nearStars.width)
        {
            SDL_Rect dstRect = {posX, posY, nearStars.width, nearStars.height};
            SDL_RenderCopy(renderer, nearStars.texture, nullptr, &dstRect);
        }   
    }
}

SDL_Point Game::renderTextCentered(std::string text, float posY, bool isTitle)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface;
    if (isTitle){
        surface = TTF_RenderUTF8_Solid(titleFont, text.c_str(), color);
    }else{
        surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int y = static_cast<int>((getWindowHeight() - surface->h) * posY);
    SDL_Rect rect = {getWindowWidth() / 2 - surface->w / 2,
                     y,
                     surface->w,
                     surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    return {rect.x + rect.w, y};  // 返回文本末尾的坐标
}

void Game::renderTextPos(std::string text, int posX, int posY, bool isLeft)
{
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderUTF8_Solid(textFont, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    if (isLeft){
        rect = {posX, posY, surface->w, surface->h};
    }else{
        rect = {getWindowWidth() - posX - surface->w, posY, surface->w, surface->h};
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void Game::insertLeaderBoard(int score, std::string name)
{
    leaderBoard.insert({score, name});
    if (leaderBoard.size() > 8){
        leaderBoard.erase(--leaderBoard.end());
    }
}

void Game::saveData()
{
    // 保存得分榜的数据
    std::ofstream file("assets/save.dat");
    if (!file.is_open()){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to open save file");
        return;
    }
    for (const auto &entry : leaderBoard){
        file << entry.first << " " << entry.second << std::endl;
    }
}

void Game::loadData()
{
    // 加载得分榜的数据
    std::ifstream file("assets/save.dat");
    if (!file.is_open()){
        SDL_Log("Failed to open save file");
        return;
    }
    leaderBoard.clear();
    int score;
    std::string name;
    while (file >> score >> name){
        leaderBoard.insert({score, name});
    }
}
// SceneMain.h
#ifndef SCENE_MAIN_H
#define SCENE_MAIN_H
#include <random>
#include <string>
#include "Scene.h"
#include "Object.h"
#include <list>
#include <SDL_mixer.h>
#include <map>
#include <SDL_ttf.h>

class Game;       

class SceneMain : public Scene {
public:
    SceneMain();
    ~SceneMain();

    void update(float deltaTime) override;
    void render() override;
    void handleEvent(SDL_Event* event) override;
    void init() override;
    void clean() override;

    void keyboardControl(float deltaTime);
    void shootPlayer();                          // 发射子弹
    void updatePlayerProjectiles(float deltaTime); // 更新子弹
    void renderPlayerProjectiles();              // 渲染子弹

    void spawEnemy();                         // 生成敌机
    void updateEnemies(float deltaTime);      // 更新敌机
    void renderEnemies();                     // 渲染敌机
    void renderEnemyProjectiles();             // 渲染敌机子弹
    void updateEnemyProjectiles(float deltaTime); // 更新敌机子弹
    void shootEnemy(Enemy* enemy);              // 敌机发射子弹
    SDL_FPoint getDirection(Enemy* enemy);      // 计算子弹方向

    void updatePlayer(float deltaTime);   // 更新玩家状态
    void enemyExplode(Enemy* enemy);      // 敌机爆炸效果

    void updateExplosions(float deltaTime);  // 更新爆炸动画
    void renderExplosions();                 // 渲染爆炸动画

    void dropItem(Enemy* enemy);
    void updateItems(float deltaTime);
    void playerGetItem(Item* item);
    void renderItems();

    void renderUI();
    void changeSceneDelayed(float deltaTime, float delay); // 延迟切换场景
    



private:
    Game &game;
    Player player;
    // 创建子弹模板
    ProjectilePlayer projectilePlayerTemplate;

    // 存储活动子弹的列表
    std::list<ProjectilePlayer*> projectilesPlayer;  

    
    // 随机数相关成员
    std::mt19937 gen;                         // 随机数生成器
    std::uniform_real_distribution<float> dis; // 随机数分布器
    
    // 敌机相关成员
    Enemy enemyTemplate;                      // 敌机模板
    std::list<Enemy*> enemies;                // 存储活动敌机的列表
    ProjectileEnemy projectileEnemyTemplate;    // 敌机子弹模板
    std::list<ProjectileEnemy*> projectilesEnemy; // 敌机子弹容器

     bool isDead = false;                 // 玩家是否死亡

    Explosion explosionTemplate;            // 爆炸动画模板
    std::list<Explosion*> explosions;       // 存储活动爆炸动画的列表

    Item itemLifeTemplate;
    std::list<Item*> items;

    Mix_Music* bgm;
    std::map<std::string, Mix_Chunk*> sounds; // 存储音效

    SDL_Texture* uiHealth; // 玩家生命值UI

    TTF_Font* scoreFont;
    int score = 0; // 玩家得分
    float timerEnd = 0.0f; // 游戏结束计时器
    
};

#endif // SCENE_MAIN_H
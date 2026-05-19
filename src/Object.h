// Object.h
#ifndef OBJECT_H
#define OBJECT_H
#include <SDL.h>

struct Player{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;
    int currentHealth = 15;      // 玩家初始生命值为20
    int maxHealth = 15;        // 玩家最大生命值为20
    Uint32 coolDown = 500;        // 射击冷却时间（毫秒）
    Uint32 lastShootTime = 0;     // 上次射击的时间
};

struct ProjectilePlayer{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 400;              // 子弹移动速度
    int damage = 1;             // 玩家子弹伤害值为1
};

struct Enemy{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;         // 敌机移动速度
    int currentHealth = 2;      // 敌机初始生命值为2
    Uint32 coolDown = 1000;     // 发射冷却时间（1秒）
    Uint32 lastShootTime = 0;   // 上次发射时间
};

struct ProjectileEnemy{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    SDL_FPoint direction = {0, 0};  // 子弹方向向量
    int width = 0;
    int height = 0;
    int speed = 400;
    int damage = 1;             // 敌机子弹伤害值为1
};

struct Explosion{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int currentFrame = 0;       // 当前帧索引
    int totlaFrame = 0;         // 总帧数
    Uint32 startTime = 0;       // 动画开始时间
    Uint32 FPS = 10;            // 帧率（每秒播放帧数）
};

// 物品类型枚举
enum class ItemType{
    Life,
    Shield,
    Time
};

// 物品结构体
struct Item{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    SDL_FPoint direction = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;
    int bounceCount = 3;
    ItemType type = ItemType::Life;
};

struct Background{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0, 0};
    float offset = 0;
    int width = 0;
    int height = 0;
    int speed = 30;
};

#endif // OBJECT_H
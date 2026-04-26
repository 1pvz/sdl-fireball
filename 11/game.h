#pragma once

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_render.h>
#include "paddle.h"
#include "ball.h"
#include "block.h"

#define MAX_COUNT SDL_SCANCODE_COUNT

struct _GameConfig {
    int width;
    int height;
    int fps;
};
typedef struct _GameConfig GameConfig;

struct _Game {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *blockImage;
    int width;
    int height;
    bool quit;
    Paddle *paddle;
    Ball *ball;
    Block **blocks;
    int numberOfBlocks;
    bool keydowns[MAX_COUNT];
    void (*actions[MAX_COUNT])(void *, SDL_Scancode sc);
    void *data[MAX_COUNT];
    bool paused;
    bool enableDrag;
    float offsetX;
    float offsetY;
    int fps;
};
typedef struct _Game Game;

Game *
Game_new(GameConfig gameConfig);

void
Game_registerAction(Game *self, SDL_Scancode key, void (*callback)(void *, SDL_Scancode sc), void *data);

bool
Game_loadLevel(Game *self, unsigned int level, SDL_Surface *blockImage);

void
Game_drawImage(Game *self, GuaImage image);

void
Game_bindEvents(Game *self);

void
Game_update(Game *self);

void
Game_draw(Game *self);

void
Game_runLoop(Game *self);

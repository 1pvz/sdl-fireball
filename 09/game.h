#pragma once

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_scancode.h>
#include "paddle.h"
#include "ball.h"
#include "block.h"

#define MAX_COUNT SDL_SCANCODE_COUNT
#define NUMBER_OF_BLOCKS 3

struct _Game {
    SDL_Window *window;
    SDL_Surface *surface;
    bool quit;
    Paddle *paddle;
    Ball *ball;
    Block *blocks[NUMBER_OF_BLOCKS];
    int numberOfBlocks;
    bool keydowns[MAX_COUNT];
    void (*actions[MAX_COUNT])(void *);
    void *data[MAX_COUNT];
};
typedef struct _Game Game;

Game *
Game_new(void);

void
Game_registerAction(Game *self, SDL_Scancode key, void (*callback)(void *), void *data);

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

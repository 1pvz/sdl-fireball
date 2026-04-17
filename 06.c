#include "SDL3/SDL_init.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string.h>
#include <assert.h>

struct _Paddle {
    SDL_Surface *image;
    int x;
    int y;
    int speed;
    void (*moveLeft)(struct _Paddle *paddle);
    void (*moveRight)(struct _Paddle *paddle);
};
typedef struct _Paddle Paddle;

SDL_Surface *
imageFromPath(const char *path) {
    SDL_Surface *image = SDL_LoadPNG(path);
    if (image == NULL) {
        SDL_Log("load image error %s\n", SDL_GetError());
    }
    return image;
}

void
Paddle_moveLeft(Paddle *self) {
    self->x -= self->speed;
}

void
Paddle_moveRight(Paddle *self) {
    self->x += self->speed;
}

Paddle *
Paddle_new(void) {
    SDL_Surface *image = imageFromPath("paddle.png");
    Paddle *o = malloc(sizeof(Paddle));
    o->image = image;
    o->x = 100;
    o->y = 250;
    o->speed = 15;
    o->moveLeft = Paddle_moveLeft;
    o->moveRight = Paddle_moveRight;
    return o;
}

// 头文件本来应该放在一起，但是现在还没有抽，先按照模块顺序组织
#define MAX_COUNT 128

struct _Game {
    SDL_Window *window;
    SDL_Surface *surface;
    Paddle *paddle;
    bool keydowns[MAX_COUNT];
    void (*actions[MAX_COUNT])(void);
};
typedef struct _Game Game;

Game *
Game_new(void) {
    Game *g = malloc(sizeof(Game));
    const int w = 640;
    const int h = 480;
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", w, h, 0);
    if (window == NULL) {
        SDL_Log("sdl create window failed %s\n", SDL_GetError());
        return NULL;
    }
    g->window = window;

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        SDL_Log("Error getting surface: %s\n", SDL_GetError());
        return NULL;
    }
    g->surface = surface;
    g->paddle = NULL;

    for (int i = 0; i < MAX_COUNT; i += 1) {
        g->keydowns[i] = false;
        g->actions[i] = NULL;
    }

    return g;
}

// typedef void (*callback)(void) action;
//
void
moveLeft(void) {
    SDL_Log("register key and move left\n");
}

void
Game_registerAction(Game *self, const char key, void (*callback)(void)) {
    assert(0 <= key && key < MAX_COUNT);
    Game *game = self;
    game->actions[key] = callback;
}

void
Game_runLoop(Game *self) {
    Game *game = self;
    Paddle *paddle = self->paddle;
    bool keyA_pressed = false;
    bool keyD_pressed = false;
    bool quit = false;

    Game_registerAction(game, 'a', moveLeft);
    while (quit == false) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                // if (event.key.key == SDLK_A) {
                //     keyA_pressed = true;
                // }
                // if (event.key.key == SDLK_D) {
                //     keyD_pressed = true;
                // }
                // int index = event.key.key;
                game->keydowns[event.key.key] = true;
            } else if (event.type == SDL_EVENT_KEY_UP) {
                // if (event.key.key == SDLK_A) {
                //     keyA_pressed = false;
                // }
                // if (event.key.key == SDLK_D) {
                //     keyD_pressed = false;
                // }
                game->keydowns[event.key.key] = false;
            }
        }

        for (int i = 0; i < MAX_COUNT; i += 1) {
            if (game->actions[i] != NULL && game->keydowns[i]) {
                game->actions[i]();
            }
        }

        // if (keyA_pressed) {
        //     paddle->moveLeft(paddle);
        // }
        // if (keyD_pressed) {
        //     paddle->moveRight(paddle);
        // }


        Uint32 color = SDL_MapSurfaceRGB(game->surface, 255, 255, 255);
        SDL_FillSurfaceRect(game->surface, NULL, color);
        SDL_Rect rect = {
            .x = paddle->x,
            .y = paddle->y,
            .w = paddle->image->w,
            .h = paddle->image->h,
        };
        SDL_BlitSurface(paddle->image, NULL, game->surface, &rect);
        SDL_UpdateWindowSurface(game->window);
    }
}

int
main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    Game *game = Game_new();
    if (game == NULL) {
        SDL_Quit();
        return 1;
    }

    Paddle *paddle = Paddle_new();
    game->paddle = paddle;
    Game_runLoop(game);

    SDL_DestroySurface(paddle->image);
    free(paddle);
    SDL_DestroyWindow(game->window);
    free(game);
    SDL_Quit();

    return 0;
}

#include "stdlib.h"
#include "assert.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include "game.h"

static const int width = 400;
static const int height = 300;

Game *
Game_new(void) {
    Game *g = malloc(sizeof(Game));
    if (g == NULL) {
        return NULL;
    }
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", width, height, 0);
    if (window == NULL) {
        SDL_Log("sdl create window failed %s\n", SDL_GetError());
        free(g);
        return NULL;
    }
    g->window = window;

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        SDL_Log("Error getting surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        free(g);
        return NULL;
    }
    g->surface = surface;
    g->quit = false;
    g->paddle = NULL;
    g->ball = NULL;

    for (int i = 0; i < NUMBER_OF_BLOCKS; i += 1) {
        g->blocks[i] = NULL;
    }
    g->numberOfBlocks = 0;

    for (int i = 0; i < MAX_COUNT; i += 1) {
        g->keydowns[i] = false;
        g->actions[i] = NULL;
        g->data[i] = NULL;
    }

    return g;
}

void
Game_registerAction(Game *self, SDL_Scancode key, void (*callback)(void *), void *data) {
    assert(key < MAX_COUNT);
    Game *game = self;
    game->actions[key] = callback;
    game->data[key] = data;
}

void
Game_drawImage(Game *self, GuaImage image) {
    SDL_Rect rect = {
        .x = image.x,
        .y = image.y,
        .w = image.image->w,
        .h = image.image->h,
    };
    Game *game = self;
    SDL_BlitSurface(image.image, NULL, game->surface, &rect);
}

void
Game_bindEvents(Game *self) {
    Game *game = self;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            game->quit = true;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            SDL_Scancode sc = event.key.scancode;
            if (sc < MAX_COUNT) {
                game->keydowns[sc] = true;
            }
        } else if (event.type == SDL_EVENT_KEY_UP) {
            SDL_Scancode sc = event.key.scancode;
            if (sc < MAX_COUNT) {
                game->keydowns[sc] = false;
            }
        }
    }
}

void
Game_update(Game *self) {
    Game *game = self;
    Ball_move(game->ball);
    if (Paddle_collide(game->paddle, game->ball->image)) {
        Ball_bounce(game->ball);
    }
    for (int i = 0; i < game->numberOfBlocks; i += 1) {
        Block *block = game->blocks[i];
        if (Block_collide(block, game->ball->image)) {
            Block_kill(block);
            Ball_bounce(game->ball);
        }
    }
}

void
Game_draw(Game *self) {
    Game *game = self;
    Paddle *paddle = game->paddle;
    Ball *ball = game->ball;
    Game_drawImage(game, paddle->image);
    Game_drawImage(game, ball->image);

    for (int i = 0; i < game->numberOfBlocks; i += 1) {
        Block *block = game->blocks[i];
        if (block->alive) {
            Game_drawImage(game, block->image);
        }
    }
    SDL_UpdateWindowSurface(game->window);
}

void
Game_runLoop(Game *self) {
    Game *game = self;

    const int delay = 1000 / 60;
    while (game->quit == false) {
        Uint32 frameStart = SDL_GetTicks();

        Game_bindEvents(game);
        for (int i = 0; i < MAX_COUNT; i += 1) {
            if (game->actions[i] != NULL && game->keydowns[i]) {
                game->actions[i](game->data[i]);
            }
        }
        // clear
        Uint64 color = SDL_MapSurfaceRGB(game->surface, 255, 255, 255);
        SDL_FillSurfaceRect(game->surface, NULL, color);
        // update
        Game_update(game);
        // draw
        Game_draw(game);

        Uint64 frameEnd = SDL_GetTicks();
        int frameTime = frameEnd - frameStart;
        if (frameTime < delay) {
            SDL_Delay(delay - frameTime);
        }
    }
}

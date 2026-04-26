#include <stdbool.h>
#include <stdlib.h>
#include "SDL3/SDL_error.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"
#include "assert.h"
#include "level_config.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_timer.h>
#include "game.h"

Game *
Game_new(GameConfig gameConfig) {
    Game *g = malloc(sizeof(Game));
    if (g == NULL) {
        return NULL;
    }
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", gameConfig.width, gameConfig.height, 0);
    if (window == NULL) {
        SDL_Log("sdl create window failed %s\n", SDL_GetError());
        free(g);
        return NULL;
    }
    g->window = window;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        SDL_Log("Error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        free(g);
        return NULL;
    }
    g->renderer = renderer;
    g->blockImage = NULL;
    g->width = gameConfig.width;
    g->height = gameConfig.height;
    g->quit = false;
    g->paddle = NULL;
    g->ball = NULL;
    g->blocks = NULL;
    g->numberOfBlocks = 0;

    for (int i = 0; i < MAX_COUNT; i += 1) {
        g->keydowns[i] = false;
        g->actions[i] = NULL;
        g->data[i] = NULL;
    }
    g->paused = false;
    g->enableDrag = false;
    g->offsetX = 0;
    g->offsetY = 0;
    g->fps = gameConfig.fps;

    return g;
}

void
Game_registerAction(Game *self, SDL_Scancode key, void (*callback)(void *, SDL_Scancode sc), void *data) {
    assert(key < MAX_COUNT);
    Game *game = self;
    game->actions[key] = callback;
    game->data[key] = data;
}

bool
Game_loadLevel(Game *self, unsigned int level, SDL_Surface *blockImage) {
    Game *game = self;

    LevelConfig levelConfig = loadLevelConfig(level - 1);
    Block **blocks = malloc(levelConfig.numberOfBlocks * sizeof(Block *));
    if (blocks == NULL) {
        SDL_Log("Failed to create blocks\n");
        return false;
    }
    Position *position = levelConfig.positions;
    int loadedCount = 0;
    for (int i = 0; i < levelConfig.numberOfBlocks; i += 1) {
        Block *block = Block_new_with_image(blockImage);
        if (block == NULL) {
            for (int j = 0; j < loadedCount; j += 1) {
                free(blocks[j]);
            }
            free(blocks);
            SDL_Log("Failed to create block\n");
            return false;
        }
        block->image.x = position[i].x;
        block->image.y = position[i].y;
        blocks[i] = block;
        loadedCount += 1;
    }

    // 创建了新的之后，再释放旧的，这样可以确保 game->blocks 要么为 NULL，要么为有效的 blocks
    for (int i = 0; i < game->numberOfBlocks; i += 1) {
        free(game->blocks[i]);
    }
    free(game->blocks);

    game->blocks = blocks;
    game->numberOfBlocks = levelConfig.numberOfBlocks;
    return true;
}

void
Game_drawImage(Game *self, GuaImage image) {
    SDL_FRect rect = {
        .x = (float)image.x,
        .y = (float)image.y,
        .w = (float)image.image->w,
        .h = (float)image.image->h,
    };
    Game *game = self;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, image.image);
    SDL_RenderTexture(game->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
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
            if (sc == SDL_SCANCODE_P) {
                game->paused = !game->paused;
            }
            unsigned int level = sc - SDL_SCANCODE_1 + 1;
            if (level >= 1 && level <= 4) {
                Game_loadLevel(game, level, game->blockImage);
            }
        } else if (event.type == SDL_EVENT_KEY_UP) {
            SDL_Scancode sc = event.key.scancode;
            if (sc < MAX_COUNT) {
                game->keydowns[sc] = false;
            }
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            float x = event.button.x;
            float y = event.button.y;
            SDL_Point point = {
                .x = x,
                .y = y,
            };
            SDL_Rect rect = {
                .x = game->ball->image.x,
                .y = game->ball->image.y,
                .w = game->ball->image.image->w,
                .h = game->ball->image.image->h,
            };
            if (SDL_PointInRect(&point, &rect)) {
                game->enableDrag = true;
                game->offsetX = x - rect.x;
                game->offsetY = y - rect.y;
            }
        } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            game->enableDrag = false;
        } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            if (game->enableDrag) {
                float x = event.motion.x;
                float y = event.motion.y;
                game->ball->image.x = x - game->offsetX;
                game->ball->image.y = y - game->offsetY;
            }
        }
    }
}

void
Game_update(Game *self) {
    Game *game = self;
    if (game->paused) {
        return;
    }
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
    SDL_RenderPresent(game->renderer);
}

void
Game_runLoop(Game *self) {
    Game *game = self;

    const int delay = 1000 / game->fps;
    while (game->quit == false) {
        Uint32 frameStart = SDL_GetTicks();

        Game_bindEvents(game);
        for (int i = 0; i < MAX_COUNT; i += 1) {
            if (game->actions[i] != NULL && game->keydowns[i]) {
                // i 是 scancode
                game->actions[i](game->data[i], i);
            }
        }
        // clear
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        SDL_RenderClear(game->renderer);
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

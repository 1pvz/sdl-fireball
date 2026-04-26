#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "10/gua_image.h"
#include "10/level_config.h"
#include "10/paddle.h"
#include "10/ball.h"
#include "10/block.h"
#include "10/game.h"
#include "10/utils.h"
#include "SDL3/SDL_scancode.h"

void
actionMoveLeft(void *data, SDL_Scancode sc) {
    Paddle *paddle = (Paddle *)data;
    Paddle_moveLeft(paddle);
}

void
actionMoveRight(void *data, SDL_Scancode sc) {
    Paddle *paddle = (Paddle *)data;
    Paddle_moveRight(paddle);
}

void
actionFire(void *data, SDL_Scancode sc) {
    Ball *ball = (Ball *)data;
    Ball_fire(ball);
}

void
actionLoadLevel(void *data, SDL_Scancode sc) {
    Game *game = data;
    unsigned int level = sc - SDL_SCANCODE_1 + 1;
    if (1 <= level && level <= 4) {
        Game_loadLevel(game, level, game->blockImage);
    }
}

int
main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    Game *game = Game_new((GameConfig) {
        .width = 400,
        .height = 300,
        .fps = 10,
    });
    if (game == NULL) {
        SDL_Quit();
        return 1;
    }

    Paddle *paddle = Paddle_new();
    if (paddle == NULL) {
        SDL_Log("Failed to create paddle\n");
        SDL_DestroyWindow(game->window);
        free(game);
        SDL_Quit();
        return 1;
    }
    paddle->game = game;
    game->paddle = paddle;

    Ball *ball = Ball_new();
    if (ball == NULL) {
        SDL_Log("Failed to create ball\n");
        SDL_DestroyWindow(game->window);
        free(game);
        SDL_Quit();
        return 1;
    }
    ball->game = game;
    game->ball = ball;

    SDL_Surface *blockImage = imageFromPath("block.png");
    if (blockImage == NULL) {
        SDL_Log("Failed to load block image\n");
        SDL_DestroyWindow(game->window);
        free(game);
        SDL_Quit();
        return 1;
    }
    game->blockImage = blockImage;
    bool loaded = Game_loadLevel(game, 1, blockImage);
    if (loaded == false) {
        SDL_DestroyWindow(game->window);
        free(game);
        SDL_Quit();
        return 1;
    }

    Game_registerAction(game, SDL_SCANCODE_A, actionMoveLeft, paddle);
    Game_registerAction(game, SDL_SCANCODE_D, actionMoveRight, paddle);
    Game_registerAction(game, SDL_SCANCODE_F, actionFire, ball);
    Game_registerAction(game, SDL_SCANCODE_1, actionLoadLevel, game);
    Game_registerAction(game, SDL_SCANCODE_2, actionLoadLevel, game);
    Game_registerAction(game, SDL_SCANCODE_3, actionLoadLevel, game);
    Game_registerAction(game, SDL_SCANCODE_4, actionLoadLevel, game);

    Game_runLoop(game);

    SDL_DestroySurface(paddle->image.image);
    free(paddle);
    SDL_DestroySurface(ball->image.image);
    free(ball);

    for (int i = 0; i < game->numberOfBlocks; i += 1) {
        Block *block = game->blocks[i];
        free(block);
        game->blocks[i] = NULL;
    }
    game->numberOfBlocks = 0;
    // 共享图片后，只需要清理一次
    SDL_DestroySurface(blockImage);
    SDL_DestroyWindow(game->window);
    free(game);
    SDL_Quit();

    return 0;
}

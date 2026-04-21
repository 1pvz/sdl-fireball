#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "09/gua_image.h"
#include "09/paddle.h"
#include "09/ball.h"
#include "09/block.h"
#include "09/game.h"
#include "09/utils.h"

void
moveLeft(void *data) {
    Paddle *paddle = (Paddle *)data;
    Paddle_moveLeft(paddle);
}

void
moveRight(void *data) {
    Paddle *paddle = (Paddle *)data;
    Paddle_moveRight(paddle);
}

void
fire(void *data) {
    Ball *ball = (Ball *)data;
    Ball_fire(ball);
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
    for (int i = 0; i < NUMBER_OF_BLOCKS; i += 1) {
        Block *block = Block_new_with_image(blockImage);
        if (block == NULL) {
            // 清理已经创建成功的 block
            for (int j = 0; j < i; j += 1) {
                free(game->blocks[j]);
            }
            SDL_Log("Failed to create block\n");
            SDL_DestroyWindow(game->window);
            free(game);
            SDL_Quit();
            return 1;
        }
        block->image.x = i * 100;
        block->image.y = 100;
        game->blocks[i] = block;
        game->numberOfBlocks += 1;
    }

    Game_registerAction(game, SDL_SCANCODE_A, moveLeft, paddle);
    Game_registerAction(game, SDL_SCANCODE_D, moveRight, paddle);
    Game_registerAction(game, SDL_SCANCODE_F, fire, ball);

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

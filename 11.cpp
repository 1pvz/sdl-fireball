#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_scancode.h>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

#include "11/gua_image.h"
#include "11/paddle.h"
#include "11/ball.h"
#include "11/block.h"
#include "11/game.h"
#include "11/utils.h"

extern "C" {
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

    // ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL3_InitForSDLRenderer(game->window, game->renderer);
    ImGui_ImplSDLRenderer3_Init(game->renderer);

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

    while (game->quit == false) {
        Uint32 frameStart = SDL_GetTicks();
        // 每帧动态计算
        const int delay = 1000 / game->fps;

        // 统一处理事件
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // ImGui 处理
            ImGui_ImplSDL3_ProcessEvent(&event);
            // 游戏处理事件
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
                SDL_Point point = { (int)x, (int)y };
                SDL_Rect rect = {
                    game->ball->image.x,
                    game->ball->image.y,
                    game->ball->image.image->w,
                    game->ball->image.image->h,
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
                    game->ball->image.x = event.motion.x - game->offsetX;
                    game->ball->image.y = event.motion.y - game->offsetY;
                }
            }
        }

        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        for (int i = 0; i < MAX_COUNT; i += 1) {
            if (game->actions[i] != NULL && game->keydowns[i]) {
                // i 是 scancode
                game->actions[i](game->data[i], (SDL_Scancode)i);
            }
        }

        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Debug");
        ImGui::Text("Ball position (%d, %d)", ball->image.x, ball->image.y);
        ImGui::SliderInt("FPS", &game->fps, 1, 60);
        ImGui::End();

        // clear
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        SDL_RenderClear(game->renderer);
        // update
        Game_update(game);
        // draw
        Game_draw(game);

        ImGui::Render();
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), game->renderer);
        SDL_RenderPresent(game->renderer);

        Uint64 frameEnd = SDL_GetTicks();
        int frameTime = frameEnd - frameStart;
        if (frameTime < delay) {
            SDL_Delay(delay - frameTime);
        }
    }

    // clear imgui
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

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

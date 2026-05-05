#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "11/gua_image.h"
#include "11/paddle.h"
#include "11/ball.h"
#include "11/block.h"
#include "11/game.h"
#include "11/utils.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_scancode.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

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

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Setup Platform/Renderer backends
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

    // Game_runLoop(game);
    while (game->quit == false) {
        Uint32 frameStart = SDL_GetTicks();
        const int delay = 1000 / game->fps;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT) {
                game->quit = true;
            }

            Game_bindEvents(game, &event);
            for (int i = 0; i < MAX_COUNT; i += 1) {
                if (game->actions[i] != NULL && game->keydowns[i]) {
                    // i 是 scancode
                    game->actions[i](game->data[i], (SDL_Scancode)i);
                }
            }
        }
        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Hello, world!");
        ImGui::Text("Ball position (%d, %d)", ball->image.x, ball->image.y);
        ImGui::Text("App average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::SliderInt("FPS", &game->fps, 1, 60);
        ImGui::End();

        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
        SDL_RenderClear(game->renderer);

        Game_update(game);
        Game_draw(game);

        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), game->renderer);
        SDL_RenderPresent(game->renderer);

        Uint64 frameEnd = SDL_GetTicks();
        int frameTime = frameEnd - frameStart;
        if (frameTime < delay) {
            SDL_Delay(delay - frameTime);
        }
    }

    // Cleanup
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

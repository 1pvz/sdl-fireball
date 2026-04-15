#include "SDL3/SDL_surface.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int
main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    const int w = 640;
    const int h = 480;
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", w, h, 0);
    if (window == NULL) {
        SDL_Log("sdl create window failed %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        SDL_Log("Error getting surface: %s\n", SDL_GetError());
        return 1;
    }

    int speed = 5;
    SDL_Surface *image = SDL_LoadPNG("paddle.png");
    SDL_Rect rect = {
        .x = 50,
        .y = 150,
        .w = image->w,
        .h = image->h,
    };

    bool keyA_pressed = false;
    bool keyD_pressed = false;
    bool quit = false;
    while (quit == false) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_A) {
                    keyA_pressed = true;
                }
                if (event.key.key == SDLK_D) {
                    keyD_pressed = true;
                }
            } else if (event.type == SDL_EVENT_KEY_UP) {
                if (event.key.key == SDLK_A) {
                    keyA_pressed = false;
                }
                if (event.key.key == SDLK_D) {
                    keyD_pressed = false;
                }
            }
        }

        if (keyA_pressed) {
            SDL_Log("key a pressed");
            rect.x -= speed;
        }
        if (keyD_pressed) {
            SDL_Log("key d pressed");
            rect.x += speed;
        }

        Uint32 color = SDL_MapSurfaceRGB(surface, 255, 255, 255);
        SDL_FillSurfaceRect(surface, NULL, color);

        SDL_BlitSurface(image, NULL, surface, &rect);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroySurface(image);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

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

    Uint32 color = SDL_MapSurfaceRGB(surface, 255, 0, 0);
    SDL_FillSurfaceRect(surface, NULL, color);
    SDL_UpdateWindowSurface(window);

    SDL_Event event;
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            break;
        }
    }

    SDL_DestroyWindow(window);
    window = NULL;
    surface = NULL;
    SDL_Quit();

    return 0;
}

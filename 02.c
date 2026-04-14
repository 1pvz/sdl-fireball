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

    SDL_Surface *image = SDL_LoadPNG("paddle.png");
    if (image == NULL) {
        SDL_Log("Error loading image: %s\n", SDL_GetError());
    }

    int x = 100;
    int y = 200;
    int speed = 15;

    SDL_Rect rect = {
        .x = x,
        .y = y,
        .w = image->w,
        .h = image->h,
    };

    bool quit = false;
    while (quit == false) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            }
            const bool *keystate = SDL_GetKeyboardState(NULL);
            if (keystate[SDL_SCANCODE_A]) {
                rect.x -= speed;
            }
            if (keystate[SDL_SCANCODE_D]) {
                rect.x += speed;
            }
        }

        // clear
        Uint32 color = SDL_MapSurfaceRGB(surface, 255, 255, 255);
        SDL_FillSurfaceRect(surface, NULL, color);

        // update
        SDL_BlitSurface(image, NULL, surface, &rect);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroySurface(image);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct _Button {
    SDL_Rect rect;
    bool is_hover;
    bool is_pressed;
    void (*on_button_click)(struct _Button *button);
};
typedef struct _Button Button;

void
onButtonClick(Button *button) {
    SDL_Log("button is_hover = %d, is_pressed = %d\n", button->is_hover, button->is_pressed);
}

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

    SDL_Rect rect = {
        .x = 100,
        .y = 100,
        .w = 120,
        .h = 50,
    };

    Button button = {
        .rect = rect,
        .is_hover = false,
        .is_pressed = false,
        .on_button_click = onButtonClick,
    };

    bool quit = false;
    while (quit == false) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;
                SDL_Point point = {
                    .x = mouseX,
                    .y = mouseY,
                };
                button.is_hover = SDL_PointInRect(&point, &button.rect);
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (button.is_hover) {
                    button.is_pressed = true;
                    if (button.on_button_click != NULL) {
                        button.on_button_click(&button);
                    }
                }
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                button.is_pressed = false;
            }
        }

        Uint32 color = SDL_MapSurfaceRGB(surface, 255, 255, 255);
        SDL_FillSurfaceRect(surface, NULL, color);

        Uint32 buttonColor;
        if (button.is_pressed) {
            buttonColor = SDL_MapSurfaceRGB(surface, 0, 0, 255);
        } else if (button.is_hover) {
            buttonColor = SDL_MapSurfaceRGB(surface, 0, 255, 0);
        } else {
            buttonColor = SDL_MapSurfaceRGB(surface, 255, 0, 0);
        }
        SDL_FillSurfaceRect(surface, &button.rect, buttonColor);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

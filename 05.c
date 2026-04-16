#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

struct _Paddle {
    SDL_Surface *image;
    int x;
    int y;
    int speed;
    void (*moveLeft)(struct _Paddle *paddle);
    void (*moveRight)(struct _Paddle *paddle);
};
typedef struct _Paddle Paddle;

SDL_Surface *
imageFromPath(const char *path) {
    SDL_Surface *image = SDL_LoadPNG(path);
    if (image == NULL) {
        SDL_Log("load image error %s\n", SDL_GetError());
    }
    return image;
}

void
Paddle_moveLeft(Paddle *self) {
    self->x -= self->speed;
}

void
Paddle_moveRight(Paddle *self) {
    self->x += self->speed;
}

Paddle *
Paddle_new(void) {
    SDL_Surface *image = imageFromPath("paddle.png");
    Paddle *o = malloc(sizeof(Paddle));
    o->image = image;
    o->x = 100;
    o->y = 250;
    o->speed = 15;
    o->moveLeft = Paddle_moveLeft;
    o->moveRight = Paddle_moveRight;
    return o;
}

// 头文件本来应该放在一起，但是现在还没有抽，先按照模块顺序组织
struct _Game {
    SDL_Window *window;
    SDL_Surface *surface;
};
typedef struct _Game Game;

Game *
Game_new(void) {
    Game *g = malloc(sizeof(Game));
    const int w = 640;
    const int h = 480;
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", w, h, 0);
    if (window == NULL) {
        SDL_Log("sdl create window failed %s\n", SDL_GetError());
        return NULL;
    }
    g->window = window;

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        SDL_Log("Error getting surface: %s\n", SDL_GetError());
        return NULL;
    }
    g->surface = surface;

    return g;
}

int
main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("Error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    Game *game = Game_new();
    if (game == NULL) {
        SDL_Quit();
        return 1;
    }

    Paddle *paddle = Paddle_new();

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
            paddle->moveLeft(paddle);
        }
        if (keyD_pressed) {
            paddle->moveRight(paddle);
        }

        Uint32 color = SDL_MapSurfaceRGB(game->surface, 255, 255, 255);
        SDL_FillSurfaceRect(game->surface, NULL, color);
        SDL_Rect rect = {
            .x = paddle->x,
            .y = paddle->y,
            .w = paddle->image->w,
            .h = paddle->image->h,
        };
        SDL_BlitSurface(paddle->image, NULL, game->surface, &rect);
        SDL_UpdateWindowSurface(game->window);
    }

    SDL_DestroySurface(paddle->image);
    free(paddle);
    SDL_DestroyWindow(game->window);
    free(game);
    SDL_Quit();

    return 0;
}

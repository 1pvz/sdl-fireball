#include <stdlib.h>
#include <assert.h>
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
    if (image == NULL) {
        return NULL;
    }
    Paddle *o = malloc(sizeof(Paddle));
    if (o == NULL) {
        SDL_DestroySurface(image);
        return NULL;
    }
    o->image = image;
    o->x = 100;
    o->y = 250;
    o->speed = 15;
    o->moveLeft = Paddle_moveLeft;
    o->moveRight = Paddle_moveRight;
    return o;
}

// 头文件本来应该放在一起，但是现在还没有抽，先按照模块顺序组织
#define MAX_COUNT 128

struct _Game {
    SDL_Window *window;
    SDL_Surface *surface;
    Paddle *paddle;
    bool keydowns[MAX_COUNT];
    void (*actions[MAX_COUNT])(void *);
    void *data[MAX_COUNT];
};
typedef struct _Game Game;

Game *
Game_new(void) {
    Game *g = malloc(sizeof(Game));
    if (g == NULL) {
        return NULL;
    }
    const int w = 640;
    const int h = 480;
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", w, h, 0);
    if (window == NULL) {
        SDL_Log("sdl create window failed %s\n", SDL_GetError());
        free(g);
        return NULL;
    }
    g->window = window;

    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) {
        SDL_Log("Error getting surface: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        free(g);
        return NULL;
    }
    g->surface = surface;
    g->paddle = NULL;

    for (int i = 0; i < MAX_COUNT; i += 1) {
        g->keydowns[i] = false;
        g->actions[i] = NULL;
        g->data[i] = NULL;
    }

    return g;
}

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
Game_registerAction(Game *self, const char key, void (*callback)(void *), void *data) {
    assert((unsigned char)key < MAX_COUNT);
    Game *game = self;
    game->actions[key] = callback;
    game->data[key] = data;
}

void
Game_runLoop(Game *self) {
    Game *game = self;
    Paddle *paddle = self->paddle;

    Game_registerAction(game, 'a', moveLeft, paddle);
    Game_registerAction(game, 'd', moveRight, paddle);

    const int delay = 1000 / 60;
    bool quit = false;
    while (quit == false) {
        Uint32 frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                unsigned char index = (unsigned char)event.key.key;
                if (index < MAX_COUNT) {
                    game->keydowns[index] = true;
                }
            } else if (event.type == SDL_EVENT_KEY_UP) {
                unsigned char index = (unsigned char)event.key.key;
                if (index < MAX_COUNT) {
                    game->keydowns[index] = false;
                }
            }
        }

        for (int i = 0; i < MAX_COUNT; i += 1) {
            if (game->actions[i] != NULL && game->keydowns[i]) {
                game->actions[i](game->data[i]);
            }
        }

        Uint64 color = SDL_MapSurfaceRGB(game->surface, 255, 255, 255);
        SDL_FillSurfaceRect(game->surface, NULL, color);
        SDL_Rect rect = {
            .x = paddle->x,
            .y = paddle->y,
            .w = paddle->image->w,
            .h = paddle->image->h,
        };
        SDL_BlitSurface(paddle->image, NULL, game->surface, &rect);
        SDL_UpdateWindowSurface(game->window);

        Uint64 frameEnd = SDL_GetTicks();
        int frameTime = frameEnd - frameStart;
        if (frameTime < delay) {
            SDL_Delay(delay - frameTime);
        }
    }
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
    if (paddle == NULL) {
        SDL_Log("Failed to create paddle\n");
        SDL_DestroyWindow(game->window);
        free(game);
        SDL_Quit();
        return 1;
    }
    game->paddle = paddle;
    Game_runLoop(game);

    SDL_DestroySurface(paddle->image);
    free(paddle);
    SDL_DestroyWindow(game->window);
    free(game);
    SDL_Quit();

    return 0;
}

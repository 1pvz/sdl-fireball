#include "SDL3/SDL_surface.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string.h>

static const int width = 400;
static const int height = 300;

struct _GuaImage {
    SDL_Surface *image;
    int x;
    int y;
};
typedef struct _GuaImage GuaImage;

struct _Paddle {
    GuaImage image;
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
    self->image.x -= self->speed;
    if (self->image.x <= 0) {
        self->image.x = 0;
    }
}

void
Paddle_moveRight(Paddle *self) {
    self->image.x += self->speed;
    if (self->image.x + self->image.image->w >= width) {
        self->image.x = width - self->image.image->w;
    }
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
    o->image.image = image;
    o->image.x = 100;
    o->image.y = 250;
    o->speed = 15;
    o->moveLeft = Paddle_moveLeft;
    o->moveRight = Paddle_moveRight;
    return o;
}

struct _Ball {
    GuaImage image;
    int speedX;
    int speedY;
    bool fired;
};
typedef struct _Ball Ball;

Ball *
Ball_new(void) {
    SDL_Surface *image = imageFromPath("ball.png");
    if (image == NULL) {
        return NULL;
    }
    Ball *o = malloc(sizeof(Ball));
    if (o == NULL) {
        SDL_DestroySurface(image);
        return NULL;
    }
    o->image.image = image;
    o->image.x = 100;
    o->image.y = 230;
    o->fired = false;
    o->speedX = 5;
    o->speedY = 5;
    return o;
}

void
Ball_fire(Ball *self) {
    self->fired = true;
}

void
Ball_move(Ball *self) {
    Ball *o = self;
    if (o->fired) {
        if (o->image.x < 0 || (o->image.x + o->image.image->w) > width) {
            o->speedX = -o->speedX;
        }
        if (o->image.y < 0 || (o->image.y + o->image.image->h) > height) {
            o->speedY = -o->speedY;
        }

        o->image.x += o->speedX;
        o->image.y += o->speedY;
    }
}

void
Ball_bounce(Ball *self) {
    self->speedY = -self->speedY;
}

struct _Block {
    GuaImage image;
    bool alive;
};
typedef struct _Block Block;

Block *
Block_new(void) {
    SDL_Surface *image = imageFromPath("block.png");
    if (image == NULL) {
        return NULL;
    }
    Block *o = malloc(sizeof(Block));
    if (o == NULL) {
        SDL_DestroySurface(image);
        return NULL;
    }
    o->image.image = image;
    o->image.x = 100;
    o->image.y = 100;
    o->alive = true;
    return o;
}

void
Block_kill(Block *self) {
    self->alive = false;
}

// 头文件本来应该放在一起，但是现在还没有抽，先按照模块顺序组织
#define MAX_COUNT SDL_SCANCODE_COUNT

struct _Game {
    SDL_Window *window;
    SDL_Surface *surface;
    bool quit;
    Paddle *paddle;
    Ball *ball;
    Block *block;
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
    SDL_Window *window = SDL_CreateWindow("Hello SDL3", width, height, 0);
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
    g->quit = false;
    g->paddle = NULL;
    g->ball = NULL;
    g->block = NULL;

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
fire(void *data) {
    Ball *ball = (Ball *)data;
    Ball_fire(ball);
}

bool
collide(GuaImage a, GuaImage b) {
    // a 在 b 的左边
    bool left = a.x + a.image->w < b.x;
    // a 在 b 的右边
    bool right = b.x + b.image->w < a.x;
    // a 在 b 的上边
    bool up = a.y + a.image->h < b.y;
    // a 在 b 的下边
    bool bottom = b.y + b.image->h < a.y;

    return !(left || right || up || bottom);
}

void
Game_registerAction(Game *self, SDL_Scancode key, void (*callback)(void *), void *data) {
    assert(key < MAX_COUNT);
    Game *game = self;
    game->actions[key] = callback;
    game->data[key] = data;
}

void
Game_drawImage(Game *self, GuaImage image) {
    SDL_Rect rect = {
        .x = image.x,
        .y = image.y,
        .w = image.image->w,
        .h = image.image->h,
    };
    Game *game = self;
    SDL_BlitSurface(image.image, NULL, game->surface, &rect);
}

void
Game_bindEvents(Game *self) {
    Game *game = self;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            game->quit = true;
        } else if (event.type == SDL_EVENT_KEY_DOWN) {
            SDL_Scancode sc = event.key.scancode;
            if (sc < MAX_COUNT) {
                game->keydowns[sc] = true;
            }
        } else if (event.type == SDL_EVENT_KEY_UP) {
            SDL_Scancode sc = event.key.scancode;
            if (sc < MAX_COUNT) {
                game->keydowns[sc] = false;
            }
        }
    }
}

void
Game_update(Game *self) {
    Game *game = self;
    Ball_move(game->ball);
    if (collide(game->paddle->image, game->ball->image)) {
        Ball_bounce(game->ball);
    }
    if (collide(game->block->image, game->ball->image)) {
        Block_kill(game->block);
    }
}

void
Game_draw(Game *self) {
    Game *game = self;
    Paddle *paddle = game->paddle;
    Ball *ball = game->ball;
    Block *block = game->block;
    Game_drawImage(game, paddle->image);
    Game_drawImage(game, ball->image);
    if (block->alive) {
        Game_drawImage(game, block->image);
    }
    SDL_UpdateWindowSurface(game->window);
}

void
Game_runLoop(Game *self) {
    Game *game = self;

    const int delay = 1000 / 60;
    while (game->quit == false) {
        Uint32 frameStart = SDL_GetTicks();

        Game_bindEvents(game);
        for (int i = 0; i < MAX_COUNT; i += 1) {
            if (game->actions[i] != NULL && game->keydowns[i]) {
                game->actions[i](game->data[i]);
            }
        }
        // clear
        Uint64 color = SDL_MapSurfaceRGB(game->surface, 255, 255, 255);
        SDL_FillSurfaceRect(game->surface, NULL, color);
        // update
        Game_update(game);
        // draw
        Game_draw(game);

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

    Ball *ball = Ball_new();
    if (ball == NULL) {
        SDL_Log("Failed to create ball\n");
        SDL_DestroyWindow(game->window);
        free(game);
        SDL_Quit();
        return 1;
    }
    game->ball = ball;

    Block *block = Block_new();
    if (ball == NULL) {
        SDL_Log("Failed to create ball\n");
        SDL_DestroyWindow(game->window);
        free(game);
        SDL_Quit();
        return 1;
    }
    game->block = block;

    Game_registerAction(game, SDL_SCANCODE_A, moveLeft, paddle);
    Game_registerAction(game, SDL_SCANCODE_D, moveRight, paddle);
    Game_registerAction(game, SDL_SCANCODE_F, fire, ball);

    Game_runLoop(game);

    SDL_DestroySurface(paddle->image.image);
    free(paddle);
    SDL_DestroySurface(ball->image.image);
    free(ball);
    SDL_DestroySurface(block->image.image);
    free(block);
    SDL_DestroyWindow(game->window);
    free(game);
    SDL_Quit();

    return 0;
}

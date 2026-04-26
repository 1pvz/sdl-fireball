#pragma once

#include "gua_image.h"

struct _Game;
typedef struct _Game Game;

struct _Paddle;
typedef struct _Paddle Paddle;
struct _Paddle {
    GuaImage image;
    struct _Game *game;
    int speed;
    void (*moveLeft)(Paddle *paddle);
    void (*moveRight)(Paddle *paddle);
};

void
Paddle_move(Paddle *self, int x);

void
Paddle_moveLeft(Paddle *self);

void
Paddle_moveRight(Paddle *self);

Paddle *
Paddle_new(void);

bool
Paddle_collide(Paddle *self, GuaImage image);

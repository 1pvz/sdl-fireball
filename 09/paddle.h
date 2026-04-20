#pragma once

#include "gua_image.h"

struct _Paddle {
    GuaImage image;
    int speed;
    void (*moveLeft)(struct _Paddle *paddle);
    void (*moveRight)(struct _Paddle *paddle);
};
typedef struct _Paddle Paddle;

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

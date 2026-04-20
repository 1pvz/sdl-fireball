#pragma once

#include "gua_image.h"

struct _Ball {
    GuaImage image;
    int speedX;
    int speedY;
    bool fired;
};
typedef struct _Ball Ball;

Ball *
Ball_new(void);

void
Ball_fire(Ball *self);

void
Ball_move(Ball *self);

void
Ball_bounce(Ball *self);

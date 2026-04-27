#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gua_image.h"

struct _Game;
typedef struct _Game Game;

struct _Ball;
typedef struct _Ball Ball;
struct _Ball {
    GuaImage image;
    Game *game;
    int speedX;
    int speedY;
    bool fired;
};

Ball *
Ball_new(void);

void
Ball_fire(Ball *self);

void
Ball_move(Ball *self);

void
Ball_bounce(Ball *self);

#ifdef __cplusplus
}
#endif

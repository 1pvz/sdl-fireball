#include "stdlib.h"
#include "ball.h"
#include "utils.h"

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
        if (o->image.x < 0 || (o->image.x + o->image.image->w) > 400) {
            o->speedX = -o->speedX;
        }
        if (o->image.y < 0 || (o->image.y + o->image.image->h) > 300) {
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

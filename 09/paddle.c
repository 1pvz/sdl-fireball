#include <stdlib.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_log.h>
#include "paddle.h"
#include "game.h"
#include "utils.h"

void
Paddle_move(Paddle *self, int x) {
    int width = self->game->width;
    int x1 = clamp(x, 0, width - self->image.image->w);
    self->image.x = x1;
}

void
Paddle_moveLeft(Paddle *self) {
    Paddle_move(self, self->image.x - self->speed);
}

void
Paddle_moveRight(Paddle *self) {
    Paddle_move(self, self->image.x + self->speed);
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
    o->game = NULL;
    o->speed = 15;
    o->moveLeft = Paddle_moveLeft;
    o->moveRight = Paddle_moveRight;
    return o;
}

bool
Paddle_collide(Paddle *self, GuaImage image) {
    return collide(self->image, image);
}

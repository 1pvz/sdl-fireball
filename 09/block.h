#pragma once

#include "gua_image.h"

struct _Block {
    GuaImage image;
    bool alive;
};
typedef struct _Block Block;

Block *
Block_new(void);

Block *
Block_new_with_image(SDL_Surface *image);

void
Block_kill(Block *self);

bool
Block_collide(Block *self, GuaImage image);

#include "stdlib.h"
#include "block.h"
#include "utils.h"

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

Block *
Block_new_with_image(SDL_Surface *image) {
    Block *o = malloc(sizeof(Block));
    if (o == NULL) {
        // 多个 block 共享图片，清理工作放在最后
        // SDL_DestroySurface(image);
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

bool
Block_collide(Block *self, GuaImage image) {
    return self->alive && collide(self->image, image);
}

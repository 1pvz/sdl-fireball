#include "utils.h"
#include <SDL3/SDL_log.h>

SDL_Surface *
imageFromPath(const char *path) {
    SDL_Surface *image = SDL_LoadPNG(path);
    if (image == NULL) {
        SDL_Log("load image error %s\n", SDL_GetError());
    }
    return image;
}

int
clamp(int value, int min, int max) {
    if (value < min) {
        value = min;
    } else if (value > max) {
        value = max;
    }
    return value;
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

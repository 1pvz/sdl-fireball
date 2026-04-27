#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gua_image.h"

SDL_Surface *
imageFromPath(const char *path);

int
clamp(int value, int min, int max);

bool
collide(GuaImage a, GuaImage b);
#ifdef __cplusplus
}
#endif

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <SDL3/SDL_surface.h>

struct _GuaImage {
    SDL_Surface *image;
    int x;
    int y;
};
typedef struct _GuaImage GuaImage;
#ifdef __cplusplus
}
#endif

#include "level_config.h"
#include <stdlib.h>

Position level1[] = {
    (Position){
        .x = 0,
        .y = 100,
    },
    (Position){
        .x = 100,
        .y = 100,
    },
    (Position){
        .x = 200,
        .y = 100,
    },
};

LevelConfig levelConfig = {
    .numberOfBlocks = 3,
    .positions = level1,
};

LevelConfig
loadLevelConfig(void) {
    return levelConfig;
}

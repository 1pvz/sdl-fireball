#include "level_config.h"
#include <stdlib.h>

Position level1[] = {
    {0, 100},
    {100, 100},
    {200, 100},
};

LevelConfig levelConfig[] = {
    {
        .numberOfBlocks = 1,
        .positions = (Position []){
            {0, 100},
        },
    },
    {
        .numberOfBlocks = 2,
        .positions = (Position []){
            {0, 100},
            {100, 100},
        },
    },
    {
        .numberOfBlocks = 3,
        .positions = (Position []){
            {0, 100},
            {100, 100},
            {200, 100},
        },
    },
    {
        .numberOfBlocks = 4,
        .positions = (Position []){
            {0, 100},
            {100, 100},
            {200, 100},
            {0, 130},
        },
    },
};

LevelConfig
loadLevelConfig(int index) {
    return levelConfig[index];
}

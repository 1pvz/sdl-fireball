#pragma once

struct _Position {
    int x;
    int y;
};
typedef struct _Position Position;

struct _LevelConfig {
    int numberOfBlocks;
    Position *positions;
};
typedef struct _LevelConfig LevelConfig;

LevelConfig
loadLevelConfig(void);

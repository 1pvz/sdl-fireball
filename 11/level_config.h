#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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
loadLevelConfig(int index);
#ifdef __cplusplus
}
#endif

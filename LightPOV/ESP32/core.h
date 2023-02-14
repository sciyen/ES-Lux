#ifndef CORE_H
#define CORE_H

/* This header file is for the defintion of basic data transfering
 * meta data type.
 */
#include <stdint.h>
#include "config.h"


enum MODES{
    MODES_CLEAR=0, 
    MODES_PLAIN, 
    MODES_SQUARE, 
    MODES_SICKLE, 
    MODES_FAN, 
    MODES_BOXES, 
    MODES_SICKLE_ADV, 
    MODES_FAN_ADV, 
    MODES_MAP_ES, 
    MODES_MAP_ES_ZH,
    MODES_CMAP_DNA, 
    MODES_CMAP_BENSON, 
    MODES_CMAP_YEN,
    MODES_CMAP_LOVE,
    MODES_CMAP_GEAR};

enum SchedulerFunc{
    FuncNone, 
    FuncConst, 
    FuncRamp, 
    FuncTri, 
    FuncPulse, 
    FuncStep};

typedef struct _ValueParam{
    SchedulerFunc func;
    uint8_t range;
    uint8_t lower;
    uint8_t p1;
    uint8_t p2;
}ValueParam;

typedef struct _mode{
    MODES mode;
    time_t start_time;
    time_t duration;
    ValueParam XH;
    ValueParam XS;
    ValueParam XV;

    ValueParam YH;
    ValueParam YS;
    ValueParam YV;

    uint8_t param[META_PARAMETER_BUF_SIZE];
}Mode;

#endif

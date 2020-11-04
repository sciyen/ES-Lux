#ifndef CORE_H
#define CORE_H

/* This header file is for the defintion of basic data transfering
 * meta data type.
 */

#include "config.h"


enum MODES{MODES_EMPTY=0};

typedef struct _mode{
    MODES mode;
    time_t start_time;
    uint8_t param[META_PARAMETER_BUF_SIZE];
}Mode;
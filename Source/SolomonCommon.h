/*************************************************************************************************************
 * Solomon internal common things that are shared across platforms
 *************************************************************************************************************/
#ifndef __SOLOMON_INTERNAL_COMMON_H__
#define __SOLOMON_INTERNAL_COMMON_H__ (1)
#include "Solomon.h"

typedef struct SolomonWindowCommon {
    int x;
    int y;
    int w;
    int h;
    char* title;
} SolomonWindowCommon;

/**
 * We parse off into the platform specific stuff here, platform specific code has the Platform instead of
 * Solomon prefix. These are called by our trampolines after they have done all the common stuff
 */

SolomonEnum PlatformWindowCreate(void* commonHandle);

#endif  // !__SOLOMON_INTERNAL_COMMON_H__

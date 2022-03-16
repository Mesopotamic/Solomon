#include "SolomonWin32.h"

size_t SolomonWindowSize() { return sizeof(SolomonWindowWin32); }

SolomonEnum PlatformWindowCreate(void* commonHandle)
{
    SolomonWindowWin32 handle = *(SolomonWindowWin32*)commonHandle;
    return SolomonEnumSuccess;
}

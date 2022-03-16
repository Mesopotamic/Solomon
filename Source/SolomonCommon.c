#include "SolomonCommon.h"
/**
 * Put the enum translator at the bottom
 */
const char* SolomonEnumTexts[SolomonEnumCount] = {
  "Success",                                                               // SolomonEnumSuccess
  "Failed at some point in the function because of a failed malloc call",  // SolomonEnumMemAllocFail
};

const char* SolomonEnumTranslate(SolomonEnum e)
{
    if (e >= SolomonEnumCount) return "Incorrect enum passed, too large";
    if (e < 0) return "Incorrect enum passed, too small";
    return SolomonEnumTexts[e];
}

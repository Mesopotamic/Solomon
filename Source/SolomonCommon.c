#include "SolomonCommon.h"

/**
 * Here we have the trampolines, where we set the internal data up ready to be sent into the platfrom specific
 * section. i.e we don't want to implement tracking the height internally 4 different times
 *
 * In order to get the correct offset of the shared struct inside the handle, we have a fake wrapper
 */
typedef struct SolomonWindowPlatTemplate {
    SolomonWindowCommon common;
};

// Default events so we don't have to use if statements in the
void defaultSolomonKeyHandler(SolomonKey key, SolomonKeyEvent e) { return; }

/*************************************************************************************************************
 * Code that calls into platform dependant varient
 *************************************************************************************************************/

SolomonWindow SolomonWindowAllocate() { return malloc(SolomonWindowSize()); }

SolomonWindow SolomonWindowCreate(int x, int y, int w, int h, char* title)
{
    SolomonWindowCommon* temp = malloc(SolomonWindowSize());
    if (!temp) return SolomonEnumMemAllocFail;

    // Todo bounds and error check these

    temp->x = x;
    temp->y = y;
    temp->w = w;
    temp->h = h;
    temp->title = title;
    temp->keyHandler = defaultSolomonKeyHandler;

    SolomonEnum err = PlatformWindowCreate(temp);
    if (err) {
        free(temp);
        return NULL;
    }

    temp->shouldContinue = true;
    temp->windowInit = true;
    return (SolomonWindow)temp;
}

SolomonEnum SolomonWindowShow(SolomonWindow window)
{
    if (!window) return SolomonEnumSegFail;
    SolomonWindowCommon temp = *(SolomonWindowCommon*)window;

    if (!temp.windowInit) return SolomonEnumNotInit;
    return PlatformWindowShow(window);
}

SolomonEnum SolomonWindowEvaluateEvents(SolomonWindow window)
{
    // Check that the window has been initiated first
    SolomonWindowCommon temp = *(SolomonWindowCommon*)window;
    if (!temp.windowInit) return SolomonEnumNotInit;

    return PlatformWindowEvaluateEvents(window);
}

/*************************************************************************************************************
 * Code that is not platform dependant
 *************************************************************************************************************/

int SolomonWindowShouldContinue(SolomonWindow window)
{
    if (!window) return 0;
    SolomonWindowCommon* temp = window;
    return temp->shouldContinue;
}

SolomonEnum SolomonWindowScheduleClose(SolomonWindow window)
{
    if (!window) return SolomonEnumOSFail;
    SolomonWindowCommon* temp = window;
    temp->shouldContinue = false;

    // Handle one more frame of messages, since the windowing loop should continues is checked before this
    // frame of messages can play
    SolomonWindowEvaluateEvents(window);
    return SolomonEnumSuccess;
}

SolomonEnum SolomonKeyEventAttachHandler(SolomonWindow window, SolomonKeyEventHandler handler)
{
    if (!window) return SolomonEnumSegFail;
    if (!handler) return SolomonEnumSegFail;
    SolomonWindowCommon* temp = window;
    temp->keyHandler = handler;
}

/*************************************************************************************************************
 * Ugly enum translator
 *************************************************************************************************************/

const char* SolomonEnumTexts[SolomonEnumCount] = {
  "Success",                                                               // SolomonEnumSuccess
  "Failed at some point in the function because of a failed malloc call",  // SolomonEnumMemAllocFail
  "Failed because of a windowing system error, ie Win32",                  // SolomonEnumOSFail
  "Failed because a Null pointer was about to be dereferenced",            // SolomonEnumSegFail
  "Failed because a Solomon object wasn't initialised ",                   // SolomonEnumNotInit
  "Failed when using vkGetInstanceProcAddr",                               // SolomonEnumVkProcFail,
  "Failed when trying to use a platforms vkCreate*Surface",                // SolomonEnumVkSurfaceFail,
};

const char* SolomonEnumTranslate(SolomonEnum e)
{
    if (e >= SolomonEnumCount) return "Incorrect enum passed, too large";
    if (e < 0) return "Incorrect enum passed, too small";
    return SolomonEnumTexts[e];
}

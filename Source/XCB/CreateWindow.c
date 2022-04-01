#include "XCB_Common.h"

size_t PlatformWindowSize() { return sizeof(SolomonWindowXCB); }

SolomonEnum PlatformWindowCreate(void* commonHandle)
{
    // We recieved a null pointer with enough space for the internal handle
    SolomonWindowXCB* handle = commonHandle;

    // Ensure that we are connected to an X server or error out
    if (connect_to_x() != SolomonEnumSuccess) return SolomonEnumOSFail;

    // Next we need to get the root window, this comes from the setup
    // Which may have changed since we first initied the connection
    const xcb_setup_t* setup = xcb_get_setup(s_connection);
    xcb_screen_t* root = xcb_setup_roots_iterator(setup).data;

    // We can now generate an ID for the window
    handle->window_id = xcb_generate_id(s_connection);

    // Now send the window information to the Xserver
    xcb_create_window(s_connection, root->root_depth, handle->window_id, root->root, handle->com.rect.x,
                      handle->com.rect.y, handle->com.rect.w, handle->com.rect.h, 1,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, root->root_visual, XCB_CW_BACK_PIXEL,
                      &root->white_pixel);

    // Store some of the most useful cookies
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(s_connection, 0, 16, "WM_DELETE_WINDOW");
    handle->replyExit = xcb_intern_atom_reply(s_connection, cookie, NULL);

    return SolomonEnumSuccess;
}

SolomonEnum PlatformWindowShow(void* commonHandle)
{
    // In order to display the window we must tell the server that we want to map the window, and then flush
    // all of the X commands to the server so they can be processed
    xcb_map_window(s_connection, ((SolomonWindowXCB*)commonHandle)->window_id);
    xcb_flush(s_connection);
    return SolomonEnumSuccess;
}

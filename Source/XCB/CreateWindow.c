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
    xcb_create_window(                //
      s_connection,                   // Connection to the X Server
      root->root_depth,               // Window depth, for layering windows?
      handle->window_id,              // ID of window to be attached to a window
      root->root,                     // Parent window
      handle->com.rect.x,             // X coordinate of top left
      handle->com.rect.y,             // Y coordinat of top left
      handle->com.rect.w,             // Width of window
      handle->com.rect.h,             // Height of window
      1,                              // Border width
      XCB_WINDOW_CLASS_INPUT_OUTPUT,  // Tell the window to in and out output
      root->root_visual,              // ID for the new windows visual
      XCB_CW_BACK_PIXEL,              // Attribute ID to set
      &root->white_pixel              // Array of values to be set in the attributes
    );

    // Now we have to set some other attributes for the window we created
    // We could have done this all in one go when creating the window. However you have to specify them in the
    // correct order, and that's not gonna be extendable

    // Tell the window to be interested in the following type of events, EXPOSURE which is when part of the
    // window shows, client messages so that is when things like the close button gets pressed,
    uint32_t attributeValue = XCB_EVENT_MASK_EXPOSURE;
    xcb_change_window_attributes(s_connection, handle->window_id, XCB_CW_EVENT_MASK, &attributeValue);

    // We want to know to close the window, so we have to record the reply from the server when we send close
    // message. That way we know the message the server will send to us when the window closes
    xcb_intern_atom_cookie_t closeCookie = xcb_intern_atom(s_connection, 0, 16, "WM_DELETE_WINDOW");
    xcb_intern_atom_reply_t* closeReply = xcb_intern_atom_reply(s_connection, closeCookie, NULL);

    // In the same manner we need to get the reply sent to window manager messages, so that we know which atom
    // to append to
    xcb_intern_atom_cookie_t wmCookie = xcb_intern_atom(s_connection, 1, 12, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t* wmReply = xcb_intern_atom_reply(s_connection, wmCookie, NULL);

    // Now we append the atom we recieved as a reply to the close event to the list of atoms that get launched
    // from the window manager close event.
    xcb_change_property(s_connection, XCB_PROP_MODE_REPLACE, handle->window_id, wmReply->atom, XCB_ATOM_ATOM,
                        XCB_ATOM_VISUALID, 1, &closeReply->atom);

    // Store the close reply so we can identify that we've recieved a close from the server
    // free the others
    handle->closeReply = closeReply;
    free(wmReply);

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

#include <xcb/xcb.h>
#include "SolomonCommon.h"

typedef struct SolomonWindowXCB {
    SolomonWindowCommon com;
    xcb_window_t window_id;
    xcb_intern_atom_reply_t* replyExit;
} SolomonWindowXCB;

// Pointer to the global xcb connection for this application
extern xcb_connection_t* s_connection;

/**
 * @brief Ensures that we have a connection to XCB, if we don't try to establish it, or return an error code
 * @returns Solomon error code
 */
SolomonEnum connect_to_x();

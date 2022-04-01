#include "XCB_Common.h"

// Declare the xcb connection as initially null
xcb_connection_t* s_connection = NULL;

// Have we established an xcb connection successfully yet?
bool connectionEstablished = false;

SolomonEnum connect_to_x()
{
    if (connectionEstablished) return SolomonEnumSuccess;

    // Try to make an XCB connection
    s_connection = xcb_connect(NULL, NULL);

    // Check if the XCB connection failed to be made
    if (xcb_connection_has_error(s_connection)) {
        int err = xcb_connection_has_error(s_connection);

        return SolomonEnumOSFail;
    }

    return SolomonEnumSuccess;
}

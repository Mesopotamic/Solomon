#include "XCB_Common.h"

SolomonEnum PlatformWindowEvaluateEvents(SolomonWindow window)
{
    SolomonWindowXCB *handle = window;
    // How do we get an event from the X server?
    // We can use wait for event or pool events, waiting for events wil block the current thread until the
    // next event is recieved, however poll events will return null if there is no event in the queue, we want
    // to use that NULL to exit out the event loop
    xcb_generic_event_t *e;
    while (e = xcb_poll_for_event(s_connection)) {
        // Every event contains an 8bit type code, the most significant bit is set if the event was generated
        // from a sendEvent request. So we need to ensure that this bit is always set to 0 so we can properly
        // evaluate events regardless of where they came from
        switch (e->response_type & ~0x80) {
            // We've recieved a message from the client, likely to be a close request
            case XCB_CLIENT_MESSAGE: {
                xcb_client_message_event_t *client = e;
                if (client->data.data32[0] == handle->replyExit->atom) {
                    // We have recieved a message from the server which was the same reply when we sent the
                    // exit cookie
                    handle->com.shouldContinue = false;
                    return;
                }
                break;
            }
            // Unknown event
            default:
                break;
        }

        free(e);
    }

    return SolomonEnumSuccess;
}

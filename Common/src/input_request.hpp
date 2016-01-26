/******************************************************************************
/* @file Contains an enumeration with available input requests.
/* 
/* @author langenhagen
/* @version 141008
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <common.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// input_request enumeration wrapping namespace.
    namespace input_request {
        /** This enumeration stores request codes for handleable input and their key codes.
         *  Easy method for simple key codes.
         */
        enum input_request {
            NONE = 0,
            EXIT = 'q',
            PAUSE = 'p'
        };
    }

    /** Logs the keyboard commands for information purposes.
     */
    void log_keyboard_commands() {
        LOG(info) << "*** key codes ***";
        LOG(info) << "Request shutdown: '" << (char)input_request::EXIT << "'";
        LOG(info) << "Request pause: '" << (char)input_request::PAUSE << "'";
    }


    /** Handles buffered keyboard input after the processing chain finished working.
     * XXX may be unsafe in some situations.
     * @return Returns an input request to the caller.
     */
    inline input_request::input_request get_keyboard_input() {
        return (input_request::input_request)nb_getch();
    }
}
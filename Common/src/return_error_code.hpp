/******************************************************************************
/* @file Error Code enumeration as function return values.
/*
/*
/* @author langenhagen
/* @version 150624
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// return_error_code enumeration wrapping namespace.
    namespace return_error_code {
        /// Defines generic error codes intended as function return values.
        enum return_error_code {
            SUCCESS = 0,
            UNSPECIFIED_ERROR = 1,
            NAN_ERROR = 2,
            INFINITE_NUMBER_ERROR = 3
        };
    }

} // END namespace app
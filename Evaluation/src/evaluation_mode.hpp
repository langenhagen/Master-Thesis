/******************************************************************************
/* @file Enumeration with supported evaluation types.
/* 
/* Not really scalable, but hey pragmatism over dogmatism.
/*
/* @author langenhagen
/* @version 150526
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <common.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)


///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Possible evaluation modes wrapping namespace.
    namespace evaluation_mode {
        /// Possible evaluation modes.
        enum evaluation_mode {
            ERROR_MODE,
            PRECISION_RECALL,
            BAYES_INFERENCE,
            OUTLIER_BATCH
        };
    }
    
    
    /** Retrieves a evaluation_mode from a given string.
     * The check with the given string is not case sensitive.
     * Logs an error message if the string is not a supported evaluation_mode.
     * type A string containing a evaluation_mode member as a written string.
     * @return An evaluation mode, especially ERROR_MODE if the given string
     * is not supported.
     */
    evaluation_mode::evaluation_mode evaluation_mode_from_string( string& type) {
        evaluation_mode::evaluation_mode ret(evaluation_mode::ERROR_MODE);
        
        string t = to_lower(type);
        if( t.compare("precision_recall") == 0)
            ret = evaluation_mode::PRECISION_RECALL;
        else if( t.compare("bayes_inference") == 0)
            ret = evaluation_mode::BAYES_INFERENCE;
        else {
            LOG( error) << FILE_LINE << "Given string \"" << type << "\" is not a supported evaluation mode.";
        }

        return ret;
    }


    /** Generates a string with the names of the supported evaluation mode.
     * @return a string with the supported evaluation modes.
     */
    inline string evaluation_modes_string() {
        return "PRECISION_RECALL, BAYES_INFERENCE";
    }


    /** Logs the evaluation modes for information purposes.
     */
    inline void log_evaluation_mode() {
        LOG(info) << "*** supported evaluation modes ***";
        LOG(info) << evaluation_modes_string();
    }

} // END namespace app
/******************************************************************************
/* @file Enumeration with supported salient region detector types.
/* 
/* Not really scalable, but hey pragmatism over dogmatism.
/*
/* @author langenhagen
/* @version 150611
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <common.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Possible feature extractor types wrapping namespace.
    namespace detector_type {
        /// Possible feature extractor types. 
        enum detector_type {
            ERROR_TYPE,
            SALIENCY_FILTERS
        };
    }
    
    
    /** Retrieves a detector_type from a given string.
     * The check with the given string is not case sensitive.
     * Logs an error message if the string is not a supported detector_type.
     * type A string containing a detector_type member as a written string.
     * @return A salient region detector type, especially ERROR_TYPE if the given string
     * is not supported.
     */
    detector_type::detector_type detector_type_from_string( string& type) {
        detector_type::detector_type ret(detector_type::ERROR_TYPE);
        string t = to_lower(type);
        if( t.compare("saliency_filters") == 0)
            ret = detector_type::SALIENCY_FILTERS;
        else {
            LOG( error) << FILE_LINE << "Given string \"" << type << "\" is not a supported salient region detector type.";
        }
        return ret;
    }

    /** Generates a string with the names of the supported salient region detector types.
     * @return a string with the supported salient regoin detector types.
     */
    inline string detector_types_string() {
        return "SALIENCY_FILTERS";        
    }

    /** Logs the salient region detector types for information purposes.
     */
    inline void log_detector_types() {
        LOG(info) << "*** supported salient region detector types ***";
        LOG(info) << detector_types_string();
    }
}
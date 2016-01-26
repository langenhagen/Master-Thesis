/******************************************************************************
/* @file Enumeration with supported feature extractor types.
/* 
/* Not really scalable, but hey pragmatism over dogmatism.
/*
/* @author langenhagen
/* @version 141005
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <common.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// extractor_type enumeration wrapping namespace.
    namespace extractor_type {
        /// Possible feature extractor types.
        enum extractor_type {
            ERROR_TYPE,
            CONTOUR,
            HISTOGRAM,
            CONTOUR_HISTOGRAM
        };
    }
    
    
    /** Retrieves a extractor_type from a given string.
     * The check with the given string is not case sensitive.
     * Logs an error message if the string is not a supported extractor_type.
     * type A string containing a extractor_type member as a written string.
     * @return A feature extractor type, especially ERROR_TYPE if the given string
     * is not supported.
     */
    extractor_type::extractor_type extractor_from_string( string& type) {
        extractor_type::extractor_type ret(extractor_type::ERROR_TYPE);
        string t = to_lower(type);
        if( t.compare("contour") == 0)
            ret = extractor_type::CONTOUR;
        else if( t.compare("histogram") == 0)
            ret = extractor_type::HISTOGRAM;
        else if( t.compare("contour_histogram") == 0)
            ret = extractor_type::CONTOUR_HISTOGRAM;
        else {
            LOG( error) << FILE_LINE << "Given string \"" << type << "\" is not a supported feature extractor type.";
        }
        return ret;
    }

    /** @brief Generates a string with the names of the supported feature extractor types.
     * @return a string with the supported feature extractor types.
     */
    inline string extractor_types_string() {
        return "CONTOUR, HISTOGRAM, CONTOUR_HISTOGRAM";        
    }

    /** @brief Logs the feature extractor types for information purposes.
     */
    inline void log_extractor_types() {
        LOG(info) << "*** supported feature extractor types ***";
        LOG(info) << extractor_types_string();
    }
}
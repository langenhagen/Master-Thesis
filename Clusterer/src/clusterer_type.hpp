/******************************************************************************
/* @file Enumeration with supported clusterer types.
/* 
/* Not really scalable, but hey pragmatism over dogmatism.
/*
/* @author langenhagen
/* @version 150119
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

    /// Possible clusterer types wrapping namespace.
    namespace clusterer_type {
	/// Possible clusterer types.
        enum clusterer_type {
            ERROR_TYPE,
            FLANNKMEANS,
            OUTLIER,
            OPTICS
        };
    }
    
    
    /** Retrieves a clusterer_type from a given string.
     * The check with the given string is not case sensitive.
     * Logs an error message if the string is not a supported clusterer_type.
     * type A string containing a clusterer_type member as a written string.
     * @return A clusterer type, especially ERROR_TYPE if the given string
     * is not supported.
     */
    clusterer_type::clusterer_type clusterer_type_from_string( string& type) {
        clusterer_type::clusterer_type ret(clusterer_type::ERROR_TYPE);
        
        string t = to_lower(type);
        if( t.compare("flannkmeans") == 0)
            ret = clusterer_type::FLANNKMEANS;
        else if( t.compare("outlier") == 0)
            ret = clusterer_type::OUTLIER;
        else if( t.compare("optics") == 0)
            ret = clusterer_type::OPTICS;
        else {
            LOG( error) << FILE_LINE << "Given string \"" << type << "\" is not a supported clusterer type.";
        }

        return ret;
    }


    /** Generates a string with the names of the supported clusterer types.
     * @return a string with the supported clusterer types.
     */
    inline string clusterer_types_string() {
        return "FLANNKMEANS, OUTLIER, OPTICS";        
    }


    /** Logs the clusterer types for information purposes.
     */
    inline void log_clusterer_types() {
        LOG(info) << "*** supported clusterer types ***";
        LOG(info) << clusterer_types_string();
    }
}
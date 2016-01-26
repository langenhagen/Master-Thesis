/******************************************************************************
/* @file Stores global statistics of the processing chain.
/* 
/* @author langenhagen
/* @version 141015
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <common.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Holds global statistics of the processing chain.
    struct global_stats {
        /// The total number of images already processed.
        uint n_processed_images;
        /// The number of images already processed in this session.
        uint n_processed_images_in_current_session;
        /// The number of images without salient regions. U gonna hate/love me for its name.
        uint n_images_without_salient_regions;
        // The time point the application started
        chrono::steady_clock::time_point app_starting_time;
        // the overall running time for each image up to now
        timespan summed_processing_timespan;
    };


    /** Logs the given global stats.
     * @param stats The statistics to be logged.
     */
    void log( const global_stats& stats) {
        LOG(info) << stats.n_processed_images << " processed images in total";
        LOG(info) << stats.n_processed_images_in_current_session << " processed images in current session";
        LOG(info) << stats.n_images_without_salient_regions << " images without salient region in current session";
        LOG(info) << chrono::round<chrono::minutes>(chrono::steady_clock::now() - stats.app_starting_time) << " application uptime.";
        LOG(info) << chrono::duration_cast<chrono::minutes>(stats.summed_processing_timespan) << " summed processing time.";
        if( stats.n_processed_images_in_current_session != 0) {
            LOG(info) << stats.summed_processing_timespan / stats.n_processed_images_in_current_session << " average processing time in current session";
        }
    }
}
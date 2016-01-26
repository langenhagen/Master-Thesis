/******************************************************************************
/* @file Saliency detector interface.
/*
/* @author langenhagen
/* @version 150127
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <program_options.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /** @brief Saliency detector interface.
     */
    class SaliencyDetector {

    public: // vars

        /// The object's description.
        saliency_detector_description& description;

    public: // constructor & destructor

        /** Main constructor.
         * @param d The description of the saliency detector.
         */
        SaliencyDetector( saliency_detector_description& d)
            : description(d) 
        {}

        /** Desctructor.
         */
        ~SaliencyDetector()
        {}

    public: // methods
        
        /** Calculates the saliency map of some given image.
         * Calls do_saliency() internally.
         * @param image An image.
         * @return A grayscale image containing the saliency map of the given image.
         * @see SaliencyDetector::do_saliency(const Mat3b&)
         */
        Mat1b saliency( const Mat3b& image) const {
            return this->do_saliency( image);            
        }


    private: // virtual interface

        /** Does the actual saliency computation. 
         * @param image An image.
         * @return A grayscale image containing the saliency map of the given image.
         * @see SaliencyDetector::saliency(const Mat3b&)
         */
        virtual Mat1b do_saliency( const Mat3b& image) const = 0;

    };
}
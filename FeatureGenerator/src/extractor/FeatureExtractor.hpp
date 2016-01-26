/******************************************************************************
/* @file Abstract salient region feature extractor.
/*
/* @author langenhagen
/* @version 150612
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <common.hpp>
#include <return_error_code.hpp>
#include <program_options.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Interface for all feature extractors.
    class FeatureExtractor {
    
    protected: // vars

        /// The description.
        feature_extractor_description& description;
    
    public: // constructor & destructor

        /** Main constructor.
         * @param d The object's description.
         */
        FeatureExtractor( feature_extractor_description& d)
            : description(d)
        {}


        /** Destructor.
         */
        virtual ~FeatureExtractor()
        {}

    public: // methods

        /** Extract a feature vector. Internally, calls the private implementation of do_extract.
         * @param original_image The original bgr image.
         * @param saliency_map A grayscale saliency map of the same dimensions as original_image.
         * @param saliency_maks A b/w mask of the smoothed and thresholded saliency map.
         * @param contours Contours that are expected to match with the saliency_mask.
         * @param[out] o_features A feature vector that will be filled with the information 
         *        found in the salient regions in that image.
         * @return A return error code.
         * @see FeatureExtractor::do_extract( const Mat3r&, const Mat1b&, const Mat1b&, const vector<Contour>&, Vec1r&)
         */
        return_error_code::return_error_code extract( const Mat3r& original_image, const Mat1b& saliency_map, const Mat1b& saliency_mask, const vector<Contour>& contours, Vec1r& o_features) const {
            return this->do_extract( original_image, saliency_map, saliency_mask, contours, o_features);
        }

    private: // virtual interface

        /** Does the actual feature extraction.
         * @param original_image The original bgr image.
         * @param saliency_map A grayscale saliency map of the same dimensions as original_image.
         * @param saliency_maks A b/w mask of the smoothed and thresholded saliency map.
         * @param contours Contours that are expected to match with the saliency_mask.
         * @param[out] o_features A feature vector that will be filled with the information 
         *        found in the salient regions in that image.
         * @return A return error code.
         * @see FeatureExtractor::extract( const Mat3r&, const Mat1b&, const Mat1b&, const vector<Contour>&, Vec1r&)
         */
        virtual return_error_code::return_error_code do_extract( const Mat3r& original_image, const Mat1b& saliency_map, const Mat1b& saliency_mask, const vector<Contour>& contours, Vec1r& o_features) const = 0;
    };
}
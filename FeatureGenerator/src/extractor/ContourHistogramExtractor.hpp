/******************************************************************************
/* @file Contour & histogram salient region feature extractor.
/*
/* @author langenhagen
/* @version 150611
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include "FeatureExtractor.hpp"
#include "ContourExtractor.hpp"
#include "HistogramExtractor.hpp"

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /** @brief Combined shape and color histogram descriptor. 
     * Feature extractor uses a ContourExtractor and
     * a HistogramExtractor and aggregates them.
     */
    class ContourHistogramExtractor : public FeatureExtractor {

    private: // vars

        feature_extractor_description _contour_description;
        feature_extractor_description _histogram_description;

        ContourExtractor* _contour_extractor;
        HistogramExtractor* _histogram_extractor;

    public: // constructor & destructor

        /** Main constructor.
         * @param d The object's description.
         */
        ContourHistogramExtractor( feature_extractor_description& d)
            : FeatureExtractor(d), 
            _contour_description(), 
            _histogram_description() {

                check_and_resolve_input_errors();

                _contour_description.tweak_vector.clear();
                _contour_description.tweak_vector.push_back( description.tweak_vector[0]);

                _histogram_description.tweak_vector.clear();
                _histogram_description.tweak_vector.insert( 
                    _histogram_description.tweak_vector.end(), 
                    d.tweak_vector.begin()+1, 
                    d.tweak_vector.end());

                _contour_extractor   = new ContourExtractor( _contour_description); 
                _histogram_extractor = new HistogramExtractor( _histogram_description); 
        }

        /** Desctructor.
         */
        ~ContourHistogramExtractor() {
            delete _histogram_extractor;
            delete _contour_extractor; 
        }


    private: // methods

        /// @see FeatureExtractor::do_extract()
        // TODO scale the contour parts / histogram parts maybe
        virtual return_error_code::return_error_code do_extract( const Mat3r& original_image, const Mat1b& saliency_map, const Mat1b& saliency_mask, const vector<Contour>& contours, Vec1r& o_features) const {
            using namespace cv;
            assert( original_image.size() == saliency_map.size() && "original_image and saliency map must have same dimensions");
            return_error_code::return_error_code ret_contour, ret_histogram;
            o_features.clear();

            Vec1r contour_features, histogram_features;
            ret_contour   = _contour_extractor->extract( original_image, saliency_map, saliency_mask, contours, contour_features);
            ret_histogram = _histogram_extractor->extract( original_image, saliency_map, saliency_mask, contours, histogram_features);
            
            o_features.reserve( contour_features.size() + histogram_features.size());
            o_features.insert(o_features.end(), contour_features.begin(), contour_features.end());
            o_features.insert(o_features.end(), histogram_features.begin(), histogram_features.end());
            return ret_contour;
        }

        protected: // helpers

            void check_and_resolve_input_errors() const {
            Vec1r& tweak = this->description.tweak_vector;

            if( tweak.size() < 2) {
                tweak.resize(2, -1);  // if too few parameters where given
            }
        }

    };
}
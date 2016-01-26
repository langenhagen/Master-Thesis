/******************************************************************************
/* @file Histogram salient region feature extractor.
/*
/* TODO make invariant to global changes.
/*
/* @author langenhagen
/* @version 150625
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include "FeatureExtractor.hpp"

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Feature Extractor that generates HSV histograms as feature vectors.
    class HistogramExtractor : public FeatureExtractor {

    public: // constructor & destructor

        /** Main constructor.
         * @param d The object's description.
         */
        HistogramExtractor( feature_extractor_description& d)
            : FeatureExtractor(d) {

            check_and_resolve_input_errors();
        }

    private: // methods

        /// @see FeatureExtractor::do_extract()
        virtual return_error_code::return_error_code do_extract( const Mat3r& original_image, const Mat1b& saliency_map, const Mat1b& saliency_mask, const vector<Contour>& contours, Vec1r& o_features) const {
            using namespace cv;
            assert( original_image.size() == saliency_map.size() && "original_image and saliency_map must have same dimensions");
            o_features.clear();
            
            // create and split hsv image from original image
            Mat3r hsv_image;
            Mat hsv_planes[3];
            Mat1r h_hist, s_hist, v_hist;
            cvtColor(original_image, hsv_image, CV_BGR2HSV);
            split( hsv_image, hsv_planes); 
            
            // quantisation values are determined by description:
            const int h_bins = static_cast<int>(description.tweak_vector[0]),
                      s_bins = static_cast<int>(description.tweak_vector[1]), 
                      v_bins = static_cast<int>(description.tweak_vector[2]);
            
            const real h_range[] = { 0, 360 };
            const real s_range[] = { 0, 1 };
            const real v_range[] = { 0, 256 };
            
            const real* h_ranges[] = { h_range };
            const real* s_ranges[] = { s_range };
            const real* v_ranges[] = { v_range };

            // use whole image for histogram calculation?
            const bool use_whole_image_as_mask = description.tweak_vector[4] == 0 ? 0 : 1;
            const Mat1b& mask = use_whole_image_as_mask ? Mat1b() : saliency_mask;

            calcHist( &hsv_planes[0], 1, nullptr, mask, h_hist, 1, &h_bins, h_ranges);
            calcHist( &hsv_planes[1], 1, nullptr, mask, s_hist, 1, &s_bins, s_ranges);
            calcHist( &hsv_planes[2], 1, nullptr, mask, v_hist, 1, &v_bins, v_ranges);
            

            Vec1r h_histvec, s_histvec, v_histvec;
            double max;
            
            minMaxLoc( h_hist, nullptr, &max);
            for( int i=0; i<h_bins; ++i)
                h_histvec.push_back( h_hist(i) / static_cast<real>(max)); 
            
            minMaxLoc( s_hist, nullptr, &max);
            for( int i=0; i<s_bins; ++i)
                s_histvec.push_back( s_hist(i) / static_cast<real>(max));
            
            minMaxLoc( v_hist, nullptr, &max);
            for( int i=0; i<v_bins; ++i)
                v_histvec.push_back( v_hist(i) / static_cast<real>(max));

            if( this->description.tweak_vector[3] > 0) {
                //channel-wise auto correlation
                h_histvec = circular_cross_correlation_series( h_histvec, h_histvec);
                s_histvec = circular_cross_correlation_series( s_histvec, s_histvec);
                v_histvec = circular_cross_correlation_series( v_histvec, v_histvec);
            }

            o_features.insert( o_features.end(), h_histvec.begin(), h_histvec.end());
            o_features.insert( o_features.end(), s_histvec.begin(), s_histvec.end());
            o_features.insert( o_features.end(), v_histvec.begin(), v_histvec.end());

            return return_error_code::SUCCESS;
        }

    protected: // helpers


        /** Helper function that checks the description for errors
         * and logs and corrects them.
         */
        void check_and_resolve_input_errors() const {
            Vec1r& tweak = this->description.tweak_vector;

            if( tweak.size() < 5 || 
                tweak[0] < 1     ||  // h_bins (el. N+)
                tweak[1] < 1     ||  // s_bins (el. N+)
                tweak[2] < 1     ||  // v_bins (el. N+)
                tweak[3] < 0     ||  // auto_correlation (el. {0,1})
                tweak[4] < 0         // use whole image for histogram (el. {0,1})
                ) {
            
                LOG(warn) << "HistogramExtractor: Tweak vector must contain 5 parameters:\n"
                             " 0: number of hue histogram bins\n"
                             " 1: number of saturation histogram bins\n"
                             " 2: number of value histogram bins\n"
                             " 3: use channel-wise autocorrelation el. {0,1}\n"
                             " 4: use color histogram of whole image   {0,1}\n";

                         
                tweak.resize(5, -1);  // if too few parameters where given

                // h_bins
                if( tweak[0] < 1) {
                    tweak[0] = 10;
                    LOG(notify) << "Setting hue bins to " << tweak[0] << ".";
                }
                // s_bins
                if( tweak[1] < 1) {
                    tweak[1] = 10;
                    LOG(notify) << "Setting saturation bins to " << tweak[1] << ".";
                }
                // v_bins
                if( tweak[2] < 1) {
                    tweak[2] = 10;
                    LOG(notify) << "Setting hue bins to " << tweak[2] << ".";
                }
                // auto_correlation
                if( tweak[3] < 0) {
                    tweak[3] = 0;
                    LOG(notify) << "Setting autocorrelation to " << tweak[3] << ".";
                }
                // color histogram of whole image
                if( tweak[4] < 0) {
                    tweak[4] = 0;
                    LOG(notify) << "Setting usage of histogram of whole image to " << tweak[4] << ".";
                }
            }
        }

    };
}
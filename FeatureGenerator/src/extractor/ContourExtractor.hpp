/******************************************************************************
/* @file Contour salient region feature extractor.
/*
/* @author langenhagen
/* @version 150624
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include "FeatureExtractor.hpp"

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// ContourExtractor uses fourier descriptors to describe shape.
    class ContourExtractor : public FeatureExtractor {

    public: // constructor & destructor

        /** Main constructor.
         * @param d The object's description.
         */
        ContourExtractor( feature_extractor_description& d)
            : FeatureExtractor(d) {

                check_and_resolve_input_errors();
        }

    private: // methods

        /// @see FeatureExtractor::do_extract()
        /// Creates a normalized fourier descriptor and uses that as the feature vector.
        virtual return_error_code::return_error_code do_extract( const Mat3r& original_image, const Mat1b& saliency_map, const Mat1b& saliency_mask, const vector<Contour>& contours, Vec1r& o_features) const {
            using namespace cv;
            assert( original_image.size() == saliency_map.size() && "original image and saliency map must have same dimensions");
            return_error_code::return_error_code ret( return_error_code::SUCCESS);
            o_features.clear();
            
            const uint n_frequencies = static_cast<uint>(description.tweak_vector[0]);
            const Contour& biggest_contour = find_biggest_contour( contours);
            
            Mat1r fd = normalized_fourier_descriptor( biggest_contour, n_frequencies);
            // copy fourier descriptor to output-vector
            // since the first two elements of fd are really just normalized to 0 and 1 we can really just skip them
            for(int c=2; c<fd.cols; ++c) {

                // validity check for each number
                real v = fd(0,c);

                if( is_inf(v))  ret = return_error_code::INFINITE_NUMBER_ERROR;
                if( is_nan(v))  ret = return_error_code::NAN_ERROR;

                o_features.push_back(v);
            }
            return ret;
        }

        
    protected: // helpers

        /** Retrieves the biggest contour of a vector of contours.
         * @param contours A vector of contours.
         * @return A reference to the biggest contour.
         */
        const Contour& find_biggest_contour( const vector<Contour>& contours) const {
            vector<Contour>::const_iterator biggest_contour_it = contours.begin();
            double biggest_contour_area = cv::contourArea( *biggest_contour_it);
            for( auto contour_it=contours.begin()+1; contour_it!=contours.end(); ++contour_it) {
                double area = contourArea( *contour_it);
                if( area > biggest_contour_area) {
                    biggest_contour_area = area;
                    biggest_contour_it = contour_it;
                }
            }
            return *biggest_contour_it;
        }


        /** Creates a normalized fourier descriptor from a given contour.
         * The descriptor will be made translation invariant, scale-invariant.
         * Furthermore, only the magnitude of the fourier description will be kept,
         * since the phase in not rotation invariant.
         * See AIA Lecture 2 or AIA Homework 2 for details.
         * @param contour The contour from which to generate the descriptor.
         * @param n_frequencies The number of frequencies to store in the descriptor.
         */
        Mat1r normalized_fourier_descriptor( const Contour& contour, uint n_frequencies) const {
            if (n_frequencies%2 != 0)
		        n_frequencies++;
            Mat2r fd(1, static_cast<int>(contour.size()));
            Mat2r normalized_fd( 1, n_frequencies);
    
            for( int c=0; c<fd.cols; ++c) {
                fd(0, c)[0] = (float)contour[c].x;
                fd(0, c)[1] = (float)contour[c].y;
            }
            cv::dft(fd, fd, cv::DFT_COMPLEX_OUTPUT); // no complex-conjugate symmetry, produce a full-size complex output array
    
            const real radius = sqrt( pow( fd(0, 1)[0], 2) + 
                                      pow( fd(0, 1)[1], 2));

            // take only low frequencies into account  +  normalize scale
            for (uint i=0; i< n_frequencies>>2; ++i) {
                // positive
                normalized_fd(0, i)[0] = fd(0, i)[0] / radius;
                normalized_fd(0, i)[1] = fd(0, i)[1] / radius;

		        // negative --> lookat aia assignment2 page12 // lookat how opencv handles dft
                normalized_fd(0, n_frequencies-1 - i)[0] = fd(0, fd.cols-1 - i)[0] / radius;
                normalized_fd(0, n_frequencies-1 - i)[1] = fd(0, fd.cols-1 - i)[1] / radius;
            }

            // translation invariance
	        normalized_fd(0,0)[0] = 0;
	        normalized_fd(0,0)[1] = 0;

            // split frequency spectrum into real and imaginary parts.
            Mat1r planes[2];  // 0: Re(normalized_fd), 1: Im(normalized_fd)
	        split( normalized_fd, planes);

            Mat1r magnitude; // rotation invariant
            Mat1r phase;
            cartToPolar(planes[0], planes[1], magnitude, phase);

            return magnitude;
        }


        /** Helper function that checks the description for errors
         * and logs and corrects them.
         */
        void check_and_resolve_input_errors() const {
            Vec1r& tweak = this->description.tweak_vector;

            if( tweak.size() < 1 || 
                tweak[0] <= 2        // n_fourier_components (el. N+)
                ) {
            
                LOG(warn) << "ContourExtractor: Tweak vector must contain 1 parameter:\n"
                             " 0: number of superpixels > 2 (since the first two fourier components are discarded)\n";
                         
                tweak.resize(1, -1);  // if too few parameters where given

                // n_frequencies
                if( tweak[0] <= 2) {
                    tweak[0] = 10;
                    LOG(notify) << "Setting n_frequencies to " << tweak[0] << ".";
                }
            }
        }

        
    };
}
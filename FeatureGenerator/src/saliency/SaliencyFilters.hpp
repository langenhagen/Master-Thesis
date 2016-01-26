/******************************************************************************
/* @file Contains an implementation of the saliency filters method by 
/* Philipp Krähenbühl et. al.
/* http://stanford.edu/~philkr/papers/saliency_filters_cvpr_2012.pdf (140902)
/* 
/*
/* TODO everything
/*
/* @author langenhagen
/* @version 150611
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include "SaliencyDetector.hpp"
#include "saliencyfilters/saliency.h"

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS


namespace app {

    /** @brief Implementation of the SaliencyDetector interface.
     * Implements Saliency Filters algorithm by 
     * Philipp Kraehenbuehl et al..
     */
    class SaliencyFilters : public SaliencyDetector {
    private: // vars

        SaliencySettings _settings;

    public: // constructor & destructor

        /// @see SaliencyDetector::SaliencyDetector().
        SaliencyFilters( saliency_detector_description& d) 
            : SaliencyDetector(d), _settings(SaliencySettings()) {

            const Vec1r& tweak = this->description.tweak_vector;

            check_and_resolve_input_errors();

            _settings.n_superpixels_           = static_cast<int>(tweak[0]);
            _settings.n_iterations_            = static_cast<int>(tweak[1]);
            _settings.superpixel_color_weight_ = static_cast<float>(tweak[2]);
            _settings.sigma_p_                 = static_cast<float>(tweak[3]);
            _settings.sigma_c_                 = static_cast<float>(tweak[4]);
            _settings.k_                       = static_cast<float>(tweak[5]);
            _settings.min_saliency_            = static_cast<float>(tweak[6]);
            _settings.alpha_                   = static_cast<float>(tweak[7]);
            _settings.beta_                    = static_cast<float>(tweak[8]);
            _settings.upsample_                = tweak[ 9] > 0 ? true : false;
            _settings.uniqueness_              = tweak[10] > 0 ? true : false;
            _settings.distribution_            = tweak[11] > 0 ? true : false;
            _settings.filter_uniqueness_       = tweak[12] > 0 ? true : false;
            _settings.filter_distribution_     = tweak[13] > 0 ? true : false;
            _settings.use_spix_color_          = tweak[14] > 0 ? true : false;

        }

        // See SaliencyDetector::~SaliencyDetector.
        ~SaliencyFilters() {}

    private: // methods
        
        /// @see SaliencyDetector::do_saliency( const Mat3b&).
        virtual Mat1b do_saliency( const Mat3b& image) const {
            Mat1b ret;

            Saliency s(_settings);
            Mat1r saliency_mat = s.saliency( image);
            saliency_mat.convertTo(ret, CV_8UC1, 255);

            return ret;
        }

    protected: // helpers

        /** Helper function that checks the description for errors
         * and logs and corrects them.
         */
        void check_and_resolve_input_errors() const {
            Vec1r& tweak = this->description.tweak_vector;

            if( tweak.size() < 15 || 
                tweak[0] <= 0 ||                    // n_superpixels (el. N+)
                tweak[1] <= 0 ||                    // n_iterations (el. N+)
                tweak[2] < 0  || tweak[2] > 1 ||    //superpixel_color_weihgt (el. [0..1])
                tweak[5] <= 0 ||                    // k (el N+)
                tweak[6] < 0  || tweak[6] > 1       // min_saliency (el. [0..1])
                ) {
            
                LOG(warn) << "SaliencyFilters: Tweak vector must contain 15 parameters:\n"
                             " 0: number of superpixels el. N+\n"
                             " 1: number of k-means iterations for SLIC el N+\n"
                             " 2: superpixel color weight el. [0..1]\n"
                             " 3: positional sigma parameter el. R\n"
                             " 4: color sigma parameter el. R\n"
                             " 5: upsampling parameter k el. N+\n"
                             " 6: minimum saliency el. [0..1]\n"
                             " 7: alpha el. R\n"
                             " 8: beta el. R\n"
                             " 9: upsamle? el. {0,1}\n"
                             "10: use uniqueness? el. {0,1}\n"
                             "11: use distribution? el. {0,1}\n"
                             "12: filter uniqueness? el. {0,1}\n"
                             "13: filter distribution? el. {0,1}\n"
                             "14: use superpixel color? el. {0,1}";
                         
                         
                tweak.resize(15, -1);  // if too few parameters where given

                // n_superpixels
                if( tweak[0] < 0) {
                    tweak[0] = 400;
                    LOG(notify) << "Setting n_superpixels to " << tweak[0] << ".";
                }
                // n_iterations
                if( tweak[1] < 0) {
                    tweak[1] = 5;
                    LOG(notify) << "Setting n_iterations to " << tweak[1] << ".";
                }
                // superpixel_color_weihgt
                if( tweak[2] < 0 || tweak[2] > 1) {
                    tweak[2] = 0;
                    LOG(notify) << "Setting superpixel_color_weihgt to " << tweak[2] << ".";
                }
                // sigma_p
                if( tweak[3] < 0) {
                    tweak[3] = 0.25;
                    LOG(notify) << "Setting sigma_p to " << tweak[3] << ".";
                }
                // sigma_c
                if( tweak[4] < 0) {
                    tweak[4] = 20.0;
                    LOG(notify) << "Setting sigma_c to " << tweak[4] << ".";
                }
                // k
                if( tweak[5] < 0) {
                    tweak[5] = 3;
                    LOG(notify) << "Setting k to " << tweak[5] << ".";
                }
                // min_saliency
                if( tweak[6] < 0) {
                    tweak[6] = static_cast<real>(0.1);
                    LOG(notify) << "Setting min_saliency to " << tweak[6] << ".";
                }
                // alpha
                if( tweak[7] < 0) {
                    tweak[7] = static_cast<real>(1.0 / 30.0);
                    LOG(notify) << "Setting alpha to " << tweak[7] << ".";
                }
                // beta
                if( tweak[8] < 0) {
                    tweak[8] = static_cast<real>(1.0 / 30.0);
                    LOG(notify) << "Setting beta to " << tweak[8] << ".";
                }
                // upsample
                if( tweak[9] < 0) {
                    tweak[9] = 1;
                    LOG(notify) << "Setting upsample to " << tweak[9] << ".";
                }
                // uniqueness
                if( tweak[10] < 0) {
                    tweak[10] = 1;
                    LOG(notify) << "Setting uniqueness to " << tweak[10] << ".";
                }
                // distribution
                if( tweak[11] < 0) {
                    tweak[11] = 1;
                    LOG(notify) << "Setting distribution to " << tweak[11] << ".";
                }
                // filter_uniqueness
                if( tweak[12] < 0) {
                    tweak[12] = 0;
                    LOG(notify) << "Setting filter_uniqueness to " << tweak[12] << ".";
                }
                // filter_distribution
                if( tweak[13] < 0) {
                    tweak[13] = 0;
                    LOG(notify) << "Setting filter_distribution to " << tweak[13] << ".";
                }
                // use_spix_color
                if( tweak[14] < 0) {
                    tweak[14] = 0;
                    LOG(notify) << "Setting use_spix_color to " << tweak[14] << ".";
                }
            
            
            } // END IF
        }
    };
}
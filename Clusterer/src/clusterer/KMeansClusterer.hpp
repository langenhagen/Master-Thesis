/******************************************************************************
/* @file K means clusterer.
/*
/* @author langenhagen
/* @version 150127
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include "Clusterer.hpp"

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <limits>

#pragma warning(push)
#pragma warning(disable:4996) // fopen may be unsafe...
#pragma warning(disable:4267) // size_t to int..
#include <opencv2/flann/flann.hpp>
#pragma warning(pop)

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /** @brief KMeans clusterer
     * Uses opencv flann hierarchical k means.
     */
    class KMeansClusterer : public Clusterer {

    public: // constructor & destructor

        /** Main constructor.
         * @param d The description of the clusterer instance.
         */
        KMeansClusterer( clusterer_description& d) 
            : Clusterer(d) {
            check_and_resolve_input_errors();
        }

        /** Destructor.
         */
        ~KMeansClusterer()
        {}

    private: // methods

        /** @see Clusterer::do_cluster()
         */
        virtual Mat1r do_cluster( const Mat1r& features) const {
            check_and_resolve_input_errors();
            Mat1r ret;
            int n_clusters;
            Mat1r cluster_means( static_cast<int>(description.tweak_vector[0]), features.cols);

            cvflann::KMeansIndexParams kmeans_index_params( 32, -1,  cvflann::CENTERS_KMEANSPP, 0.2f);
            n_clusters = cv::flann::hierarchicalClustering<cv::flann::L2<real>>( features, cluster_means, kmeans_index_params);
            ret = Mat1r( features.rows, n_clusters, real(0));

            for( int r=0; r<features.rows; ++r) {
                // assign the probility of each feature to one cluster. k means is hard assignment.
                const cv::Mat_<real> &feat = features.row(r);
                    
                // get nearest cluster
                real min_dist = real( norm( feat, cluster_means.row(0), cv::NORM_L2));
                int nearest_cluster(0);
                for( int c=1; c<n_clusters; ++c) {
                    const real dist = real( norm( feat, cluster_means.row(c), cv::NORM_L2));
                    if( dist < min_dist) {
                        min_dist = dist;
                        nearest_cluster = c;
                    }
                }

                // assign probability 1 to nearest cluster
                ret(r, nearest_cluster) = 1;
            }

            return ret;
        }

    protected: // helpers

        /** Helper function that checks the description for errors 
         * and logs and corrects them.
         */
        void check_and_resolve_input_errors() const {
            if( this->description.tweak_vector.size() == 0 || this->description.tweak_vector[0] < 0) {
                const unsigned int n_clusters_hint = 100;
                LOG(warn) << "KMeansClusterer: Tweak vector must at least contain one positive integer hint for the number of clusters.";
                LOG(notify) << "Setting hint of cluster count to " << n_clusters_hint << ".";
                this->description.tweak_vector.clear();
                this->description.tweak_vector.push_back( (const real)n_clusters_hint);
            }
        }

    };

}
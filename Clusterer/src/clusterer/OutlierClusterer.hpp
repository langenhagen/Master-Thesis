/******************************************************************************
/* @file Outlier clusterer. Finds n feature vectors that are farthest away from
/*       all other elements.
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

#include <queue>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /** @brief Outlier clusterer. Finds orphanized elements.
     */
    class OutlierClusterer : public Clusterer {

    public: // constructor & destructor

        /** Main constructor.
         * @param d The description of the clusterer instance.
         */
        OutlierClusterer( clusterer_description& d) 
            : Clusterer(d) {
            check_and_resolve_input_errors();
        }

        /** Destructor.
         */
        ~OutlierClusterer()
        {}

    public: // methods

        /** @see Clusterer::do_cluster()
         */
        virtual Mat1r do_cluster( const Mat1r& features) const {
            typedef std::pair<unsigned int /*index*/, app::real /*distance to nearest neighbor*/> Outlier;
            struct OutlierGreater {
                inline bool operator()(const Outlier &a, const Outlier &b) const { return a.second > b.second; }
            };
            typedef std::priority_queue<Outlier, std::vector<Outlier>, OutlierGreater> OutlierPriorityQueue;
            
            check_and_resolve_input_errors();
            Mat1r ret(features.rows, 2);
            ret.col(0) = 1;
            ret.col(1) = 0;
            const unsigned int n_outliers( static_cast<uint>(this->description.tweak_vector[0]));
            assert( n_outliers < static_cast<uint>(features.rows) && "The number of outliers must be smaller than the number of feature points");
            
            OutlierPriorityQueue outliers;

            for( int i=0; i<features.rows; ++i) {
                register Outlier o( i, std::numeric_limits<real>::max());
                for( int j=i+1; j<features.rows; ++j) {
       
                    const real dist( static_cast<real>(norm( features.row(i), features.row(j), cv::NORM_L2)));
                    if( dist < o.second)
                        o.second = dist;
                }
                
                outliers.push( o);

                if( outliers.size() > n_outliers)
                    outliers.pop();

                if( i%1000 == 0) {
                    LOG(info) << "OutlierClusterer: " << i << " / " << features.rows << " feature-vectors visited.";
                }
            }

            while( !outliers.empty()) {
                const Outlier& o = outliers.top();
                ret( o.first, 0) = 0;
                ret( o.first, 1) = 1;
                outliers.pop();
            }

            return ret;
        }


    protected: // helpers

        /** Helper function that checks the description for errors 
         * and logs and corrects them.
         */
        void check_and_resolve_input_errors() const {
            if( this->description.tweak_vector.size() == 0 || this->description.tweak_vector[0] < 0) {
                const unsigned int n_outliers = 100;
                LOG(warn) << "OutlierClusterer: Tweak vector must at least contain one positive integer number for the number of outliers to be found.";
                LOG(notify) << "Setting number of outlier to " << n_outliers << ".";
                this->description.tweak_vector.clear();
                this->description.tweak_vector.push_back( (const real)n_outliers);
            }
        }

    };

}
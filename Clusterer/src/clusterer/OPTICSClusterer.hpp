/******************************************************************************
/* @file OPTICS clusterer. Uses the OPTICS ordering presented by 
/*       Ankerst, Breunig, Kriegel & Sander to cluster the dataset.
/*       (http://fogo.dbs.ifi.lmu.de/Publikationen/Papers/OPTICS.pdf)
/*
/* @author langenhagen
/* @version 150520
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include "Clusterer.hpp"
#include "OPTICS/optics.hpp"

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <Persistence1D/src/persistence1d/persistence1d.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /** @brief OPTCIS clusterer. Density based clustering.
     */
    class OPTICSClusterer : public Clusterer {

    private: // types

        /// describes the mode that is to be used by optics
        enum optics_mode {
            N_CLUSTERS  = 0,    ///< find exactly n clusters (or less...)
            PERSISTENCE = 1     ///< find k most persistent clusters
        };

    public: // constructor & destructor

        /** Main constructor.
         * @param d The description of the clusterer instance.
         */
        OPTICSClusterer( clusterer_description& d) 
            : Clusterer(d)
        {}

        /** Destructor.
         */
        ~OPTICSClusterer()
        {}

    public: // methods

        /** @see Clusterer::do_cluster()
         * XXX maybe opt to put outliers to nearest cluster, instead of its own
         */
        virtual Mat1r do_cluster( const Mat1r& features) const {
            typedef OPTICS::LabelledDataPoint<int> OPTICSPoint;
            const uint n_features = features.rows;
            check_and_resolve_input_errors( n_features);
            Mat1r ret;
            
            const Vec1r& tweak = this->description.tweak_vector;

            const optics_mode mode               = tweak[0] == N_CLUSTERS ? N_CLUSTERS : PERSISTENCE; // not schoen but selten
            const OPTICS::real eps               = tweak[1]; 
            const uint min_pts                   = static_cast<uint>(tweak[2]);
            const uint n_clusters                = static_cast<uint>(tweak[3]);
            const OPTICS::real persistence       = tweak[3];
            const OPTICS::real outlier_threshold = tweak[4];
            
            // assemble data
            OPTICS::DataVector db;
            for( int r=0; r<features.rows; ++r) {
                OPTICSPoint* p = new OPTICSPoint( r);
                vector<OPTICS::real>& data = p->data();
                for( int c=0; c<features.cols; ++c)
                    data.push_back( features(r,c));
                db.push_back(p);
            }

            // run optics
            uint n_processed = 0;
            OPTICS::DataVector result = 
                OPTICS::optics( db, 
                                eps, 
                                min_pts, 
                                [&n_processed, &n_features](const OPTICS::DataPoint* p){
                                    n_processed++;
                                    if( n_processed % 100 == 0) {
                                        const real percent = static_cast<int>( 100.0 * n_processed / n_features * 100 + 0.5) / 100.0f;
                                        LOG(info) << "OPTICSClusterer: " << percent << "% (" << n_processed << '/' << n_features << ") done.";
                                    }
                                });

            // extract reachability distances
            vector<OPTICS::real> reachabilities;
            Vec1i ordered_indices;
            std::for_each( result.begin(), 
                           result.end(), 
                           [&reachabilities, &ordered_indices]( const OPTICS::DataPoint* p) {
                                const OPTICSPoint* lp = dynamic_cast<const OPTICSPoint*>(p);
                                reachabilities.push_back( p->reachability_distance());
                                ordered_indices.push_back( lp->label());
                           });


            const string reachability_distances_fname = "optics_reachability_distances.txt";
            const string ordered_indices_fname = "optics_ordered_feature_indices.txt";
            LOG(info) << "OPTICSClusterer: Writing reachability histogram to \"" << reachability_distances_fname << "\"...";
            to_file(reachability_distances_fname, reachabilities);
            LOG(info) << "OPTICSClusterer: Writing ordered indices to \"" << ordered_indices_fname << "\"...";
            to_file(ordered_indices_fname, ordered_indices);


            // find clusters
            Vec1UInt cluster_borders;
            if( mode == N_CLUSTERS) {
                cluster_borders = find_k_histogram_peaks( reachabilities, n_clusters);
            } else if( mode == PERSISTENCE) {
                cluster_borders = find_histogram_peaks( reachabilities, persistence);
            } else {
                //should never happen
                LOG(error) << "OPTICSClusterer: Not able to handle the mode \"" << mode << "\".";
            }
            
            std::sort( cluster_borders.begin(), cluster_borders.end());
            vector<OPTICS::DataVector> clusters = OPTICS::extract_clusters( result, cluster_borders, outlier_threshold);


            // set 1 on assigned cluster position
            ret = Mat1r( n_features, static_cast<int>(clusters.size()), real(0));
            for( uint i=0; i<clusters.size(); ++i) {
                const OPTICS::DataVector& cluster_i = clusters[i];
                for( uint j=0; j<cluster_i.size(); ++j) {
                    const OPTICSPoint* p = dynamic_cast<OPTICSPoint*>(cluster_i[j]);
                    auto tt = p->label();
                    ret( p->label(), i) = 1;
                }
            }

            // clean up
            for( auto it=db.begin(); it!=db.end(); ++it) {
                delete *it;
            }


            return ret;
        }


    protected: // helpers

        /** Helper function that checks the description for errors 
         * and logs and corrects them.
         * @param n_features The number of features that will be processed by the clustering method.
         */
        void check_and_resolve_input_errors( const uint n_features) const {
            Vec1r& tweak = this->description.tweak_vector;

            if( tweak.size() < 5 || 
                tweak[0] != N_CLUSTERS && tweak[0] != PERSISTENCE ||    // mode (N_CLUSTERS or PERSISTENCE)
                tweak[1] <= 0 ||    // epsilon (el. R+)
                tweak[2] <= 0 ||    // min_pts (el. N+)
                tweak[3] <= 0 ||    // n_clusters (el N+) or persistence (el. R+)
                tweak[4] <= 0       // outlier_threshold (el. R+)
                ) {
                
                const OPTICS::real max = std::numeric_limits<OPTICS::real>::max();
                const real min_pts_default_percentage = 1;

                LOG(warn) << "OPTICSClusterer: Tweak vector must contain 5 parameters:\n"
                             "0: 1 number specifying the behaviour: 0: find n clusters    or    1: find all clusters with high persistence at peaks\n"
                             "1: the epsilon optics parameter, if zero or negative, epsilon will be set to " << max << "\n"
                             "2: the min_pts optics parameter, a positive integer; will otherwise be set to " << min_pts_default_percentage << "% of the size of the input data set\n"
                             "3: the number of clusters, if parameter 0 is set to '0'    or    the persistence value if parameter 0 is set to '1'\n"
                             "4: the outlier threshold: must be positive, will be set to " << max << " otherwise";
                
                tweak.resize(5, -1);  // if too few parameters where given

                // mode
                if( tweak[0] != N_CLUSTERS && tweak[0] != PERSISTENCE) {
                    tweak[0] = N_CLUSTERS;
                    LOG(notify) << "Setting OPTICS mode to N_CLUSTERS aka " << tweak[0] << ".";
                }
                // epsilon
                if( tweak[1] <=0) {
                    tweak[1] = max;
                    LOG(notify) << "Setting epsilon to " << tweak[1] << ".";
                }
                // min_pts
                if( tweak[2] <=0) {
                    tweak[2] = static_cast<real>(n_features) / 100 * min_pts_default_percentage;
                    LOG(notify) << "Setting min_pts to " << tweak[2] << ".";
                }
                // n_clusters / persistence
                if( tweak[3] <=0) {
                    if( tweak[0] == N_CLUSTERS) {
                        tweak[3] = 1;
                        LOG(notify) << "Setting n_clusters to " << tweak[3] << ".";
                    } else {
                        tweak[3] = 0;
                        LOG(notify) << "Setting persistence value to " << tweak[3] << ".";
                    }
                }
                // outlier_threshold
                if( tweak[4] <=0) {
                    tweak[4] = max;
                    LOG(notify) << "Setting outlier threshold value to " << tweak[4] << ".";
                }
                
            } // END IF
        }


        /** Given the OPTICS ordered output, finds the k most persistent maxima peaks 
         * of the reachability distances, which are presumably cluster-borders.
         * @param reachabilities The OPTICS ordered reachability distances of the DataPoints 
         *        that where the input of the OPTICS function.
         * @param n_clusters the number of clusters that shall is we want to extract.
         *        The n_clusters-1 most persistent histogram peaks are assumed to be their borders.
         *        If less than n_clusters-1 histogram peaks exist, all peak incides will be returned.
         * @return All the n_clusters-1 biggest histogram peak indices .
         *         The indices are ordered in descending order to the persistence of the peaks at these positions.
         * @see optics()
         */
        Vec1UInt find_k_histogram_peaks( const vector<OPTICS::real>& reachabilities,
                                         const uint n_clusters) const {
            Vec1UInt ret;

            p1d::Persistence1D p;
            p.RunPersistence(reachabilities);
            vector<p1d::TPairedExtrema> extrema;
            p.GetPairedExtrema(extrema, 0);
            
            unsigned int n=0;
            for( int i=static_cast<int>(extrema.size())-1; i>=0 && n<n_clusters; --i, ++n)
                ret.push_back( extrema[i].MaxIndex);

            return ret;
        }


        /** Given the OPTICS ordered output, finds all maxima peaks with a persistence 
         * greater than a given threshold. These are presumably cluster-borders.
         * @param reachabilities The OPTICS ordered reachability distances of the DataPoints 
         *        that where the input of the OPTICS function.
         * @param persistence The persistence of the histogram peaks to retain.
         * @return All histogram peak indices that are above the given persistence threshold.
         *         The indices are ordered in ascending order to the persistence of the peaks at these positions.
         * @see optics()
         */
        Vec1UInt find_histogram_peaks( const vector<OPTICS::real>& reachabilities, 
                                       const OPTICS::real persistence) const {
            Vec1UInt ret;

            p1d::Persistence1D p;
            p.RunPersistence(reachabilities);
            vector<p1d::TPairedExtrema> extrema;
            p.GetPairedExtrema(extrema, persistence);

            std::for_each(extrema.begin(), extrema.end(), [&ret]( const p1d::TPairedExtrema& e) { ret.push_back( e.MaxIndex); });

            return ret;
        }

    };

}
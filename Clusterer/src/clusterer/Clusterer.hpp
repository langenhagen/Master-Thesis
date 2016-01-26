/******************************************************************************
/* @file Clusterer interface.
/*
/* @author langenhagen
/* @version 150127
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <program_options.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Interface for clusterer classes.
    class Clusterer {

    protected: // vars

        // The object's description.
        clusterer_description& description;

    public: // constructor & destructor

        /** Main constructor.
         * @param d The description of the clusterer instance.
         */
        Clusterer( clusterer_description& d) 
            : description(d)
        {}

        /** Destructor.
         */
        virtual ~Clusterer()
        {}

    public: // static methods

        /** Assigns every every row-index of the given probability-matrix to a distinct cluster index.
         * @param probabilities A matrix witch rows represent the probabilities of one feature to belong to a certain cluster.
         * @return Returns a vector of indices that indicate the cluster index el. {0, .., probabilities.rows}.
         */
        static vector<uint> assign( const Mat1r& probabilities) {
            vector<uint> ret;
            ret.reserve( probabilities.rows);
            
            for( int r=0; r<probabilities.rows; ++r) {
                double min, max;
                cv::Point min_loc, max_loc;

                cv::minMaxLoc( probabilities.row(r), &min, &max, &min_loc, &max_loc);
                ret.push_back( max_loc.x);
            }
            return ret;
        }

    public: // methods

        /** Clusters the given features, calls a private implementation of do_cluster.
         * @param features The row-wise feature vectors to be clustered.
         * @return A matrix that contains row-wise probabilities for each feature 
         *         to belong to one class el. [0,1]. The matrix has as much rows 
         *         as the given features and as much colums as clusters created.
         * @see Clusterer::do_cluster( const Mat3r&)
         */
        Mat1r cluster( const Mat1r& features) const {
            return this->do_cluster( features);
        }


    private: // virtual interface


        /** Does the actual clustering.
         * @param features The row-wise feature vectors to be clustered.
         * @return A matrix that contains row-wise probabilities for each feature 
         *         to belong to one class el. [0,1]. The matrix has as much rows 
         *         as the given features and as much colums as clusters created.
         * @see Clusterer::cluster( const Mat31&)
         */
        virtual Mat1r do_cluster( const Mat1r& features) const = 0;
    };


}
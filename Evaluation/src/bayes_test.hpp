/******************************************************************************
/* @file Bayes inference tests for the Evaluation application.
/* 
/* The code is not intended to be efficient, but to work.
/* Speed shouldn't be a matter here.
/*
/* @author langenhagen
/* @version 150527
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

    void eval_bayes( const program_options& params);
    Vec1UInt assign_membership( const Mat1r& probabilities);


    /** Runs the bayesian inference test.
     * May log errors and even end the application in case of severe error.
     * @param params The program options.
     */
    void eval_bayes( const program_options& params) {
    
        LOG(info) << "Loading class membership probabilities...";
        Mat1r membership_probabilities;
        exit_if_false( from_file( params.membership_probabilities_file, membership_probabilities), RETURN_CODE::IO_ERROR); 
        
        const uint n_clusters = membership_probabilities.cols;
        const uint n_features = membership_probabilities.rows;

        LOG(info) << "# features: " << n_features;
        LOG(info) << "# clusters: " << n_clusters;
        
        LOG(info) << "Assigning membership mappings ...";
        Vec1UInt membership_mappings = assign_membership( membership_probabilities);
        vector<Mat1r> segmented_membership_probabilities( n_clusters);
        for(uint r=0; r<n_features; ++r) {
            segmented_membership_probabilities[membership_mappings[r]].push_back( membership_probabilities.row(r));
        }
        // === overall: calc P(C=c|X=x) ===

        LOG(info) << "Calculating likelihoods  P(X=x|C=c)...";
        // calc independent #classes-gaussian distributions ..for each class.
        // likelihoods: P(X=x|C=c) = p(X_1=x_1|C=c)*p(X_2=x_2|C=c)*..   [class_idx][dimension]
        vector<vector<RealGaussian>> likelihoods( n_clusters);
        for( uint i=0; i<n_clusters; ++i) {

            const Mat1r& segment = segmented_membership_probabilities[i];

            // Check if segment matrix has full rank, which is important for checking
            Mat1r singular_values;
            cv::SVD::compute(segment, singular_values, cv::SVD::NO_UV);
            if( std::count((app::real*)singular_values.datastart, (app::real*)singular_values.dataend, 0) > 0 ) {
                LOG(warn) << "Segment matrix for class: " << i << " doesn't have full rank."; 
            }

            for( uint j=0; j<n_clusters; ++j) {
                cv::Mat1d var, mean; 
                cv::calcCovarMatrix(segment.col(j), var, mean, CV_COVAR_NORMAL | CV_COVAR_ROWS); // splitted column-wise bc we only need the diagonals of the covar-matrix
            
                const real mean_value = static_cast<real>(mean(0,0));
                const real var_value  = static_cast<real>(var(0,0)/n_clusters);

                likelihoods[i].push_back( RealGaussian( mean_value, var_value));

                ANXIOUS_VALIDITY_CHECK( mean(0,0), "likelihoods - mean(0,0)");
                ANXIOUS_VALIDITY_CHECK( var(0,0)/n_clusters, "likelihoods - var(0,0)/n_clusters");
            }
        }

        LOG(info) << "Calculating priors P(C=c)...";
        Mat1r priors(1, n_clusters);
        for(uint i=0; i<n_clusters; ++i) {
            priors(0, i) = static_cast<real>(segmented_membership_probabilities[i].rows);
        }
        priors /= n_features;
    
        // do some numeric validity checks
        for(uint i=0; i<n_clusters; ++i) {
            std::stringstream ss;
            ss << "priors(0, " << i << ")";
            ANXIOUS_VALIDITY_CHECK( priors(0,i), ss.str());
        }


        LOG(info) << "Calculating (unnormalized) posteriors P(C=c|X=x)...";
        Mat1r posteriors( n_features, n_clusters);
        for(uint cluster=0; cluster<n_clusters; ++cluster) {

            const real prior = priors(0, cluster);

            for(uint feature=0; feature<n_features; ++feature) {
            
                real likelihood(1);
                for( uint dim=0; dim<n_clusters; ++dim) {
                    real likelihood_1_dim = static_cast<real>( likelihoods[cluster][dim]( membership_probabilities(feature,dim))); 
                    likelihood *= likelihood_1_dim;
                
                    std::stringstream ss;
                    ss << "likelihood-1-dim: " << dim;
                    ANXIOUS_VALIDITY_CHECK( likelihood_1_dim, ss.str());
                    ss.str("");
                    ss << "likelihood - after " << dim << " multiplications";
                    ANXIOUS_VALIDITY_CHECK( likelihood, ss.str());
                }

                posteriors( feature, cluster) = likelihood*prior;
                ANXIOUS_VALIDITY_CHECK( likelihood*prior, "likelihood*prior");
            }
        }

        //TODO normalize TODO ????
    
        LOG(info) << "Writing posteriors to \"" << params.posteriors_file << "\"...";
        to_file( params.posteriors_file, posteriors);
    
    }


    /** Hard assigns membership mappings given the class-membership probabilities.
     * @param probabilities A matrix storing the probabilities of each datum 
     *        to belong to one class. The probability for feature f to belong to class c
     *        must be stored in probabilities( f, c ) with the row index specifying the feature
     *        and the column index specifying the class.
     * @return A vector of length of all probabilities, e.g. the number of all data points.
     *         The vector contains numbers each index to the assigned class id.
     */
    Vec1UInt assign_membership( const Mat1r& probabilities) {
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

} // END namespace app
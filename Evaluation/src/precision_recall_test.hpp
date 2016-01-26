/******************************************************************************
/* @file Precision/recall tests for the Evaluation application.
/* 
/* The code is not intended to be efficient, but to work.
/* Speed shouldn't be a matter here.
/*
/* @author langenhagen
/* @version 150615
/******************************************************************************/
#pragma once 

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <program_options.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <tuple>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Tuple of assigned class name, true, positives, false positives and false negatives
    typedef std::tuple< string /*classname*/, 
                        uint   /*true positives*/, 
                        uint   /*false positives*/, 
                        uint   /*false negatives*/,
                        real   /*precision*/,
                        real   /*recall*/> cluster_info_t;


    void eval_precision_recall( const program_options& params);
    std::ostream& operator<< (std::ostream& stream, const cluster_info_t& o);


    /** Runs the precision/recall test.
     * May log errors and even end the application in case of severe error.
     * @param params The program options.
     */
    void eval_precision_recall( const program_options& params) {

        LOG(info) << "Loading class membership mappings...";
        Vec1UInt membership_mappings;
        Vec1str cluster_files;
    
        exit_if_false( from_file( params.membership_mappings_file, membership_mappings), RETURN_CODE::IO_ERROR);
        exit_if_false( from_file( params.cluster_file_paths_file,  cluster_files),       RETURN_CODE::IO_ERROR);

        const uint n_features = static_cast<uint>(membership_mappings.size());
        const uint n_clusters = static_cast<uint>(cluster_files.size());

        LOG(info) << "# features: " << n_features;
        LOG(info) << "# clusters: " << n_clusters;


        LOG( info) << "calculating precision / recall ...";

        // find best class for each cluster
        vector<cluster_info_t> class_mapping;

        for( uint i=0; i<n_clusters; ++i) {

            Vec1str current_cluster_image_paths;
            Vec1str current_cluster_real_image_classes;
            std::map<string, uint> class_votes;
            from_file( cluster_files[i], current_cluster_image_paths);
        

            for( auto it=current_cluster_image_paths.begin(); it!=current_cluster_image_paths.end(); ++it) {
            
                const string class_name = bfs::path(*it).parent_path().filename().string();

                current_cluster_real_image_classes.push_back( class_name); 

                const auto map_it = class_votes.find( class_name);
                if( map_it == class_votes.end())
                    class_votes[class_name] = 1;
                else
                    map_it->second += 1;
            }
            
            const auto max_it = std::max_element( class_votes.begin(), 
                                                  class_votes.end(), 
                                                  []( const std::pair<string, int>& p, const std::pair<string, int>& q) { 
                                                      return p.second < q.second; 
                                                  });
            
            if( max_it == class_votes.end()) {
                // ***cluster empty *** (yes, that can happen!)
                continue;
            }

            const string assigned_class( max_it->first);

            const uint n_retrieved_images( static_cast<uint>(current_cluster_image_paths.size()));
            uint false_positives(0);
            uint true_positives(0);
            uint false_negatives(0);

            // find true positives, false positives
            for( auto it=current_cluster_real_image_classes.begin(); it!=current_cluster_real_image_classes.end(); ++it) {
                const string& real_class = *it;
                if( assigned_class.compare( real_class) == 0) {
                    ++true_positives;
                } else {
                    ++false_positives;
                }
            }
            // false negatives
            std::stringstream folder_name;
            folder_name << params.image_db_directory << '/' << assigned_class;
            bfs::path folder_path( folder_name.str());
            assert( bfs::exists( folder_path) && "the directory must exist.");

        
            uint n_relevant_images(0);
            for( bfs::directory_iterator it(folder_path); it!=bfs::directory_iterator(); ++it) {
                bfs::path p(*it);
                p.make_preferred();
                
                if( !p.has_extension() || !is_image_filetype_supported( p.extension().string()))
                    continue;

                ++n_relevant_images;
            
                Vec1str::iterator pos = std::find( current_cluster_image_paths.begin(), current_cluster_image_paths.end(), p.string());
                if( pos == current_cluster_image_paths.end())
                    ++false_negatives;
            }
            assert( true_positives + false_negatives == n_relevant_images && "number of relevant images must be identical to the number of true positivies and false positives");

            // *** found true positivies, false positives, false negatives for cluster i ***
            // calc precision/recall for each class
            const real precision = static_cast<real>(true_positives) / n_retrieved_images;
            const real recall    = static_cast<real>(true_positives) / n_relevant_images;

            class_mapping.push_back( cluster_info_t( assigned_class, true_positives, false_positives, false_negatives, precision, recall));
        }

        real avg_true_positives(0);
        real avg_false_positives(0);
        real avg_false_negatives(0);
        real avg_precision(0);
        real avg_recall(0);

        LOG( info) << "<class name> <true positives> <false positives> <false negatives> <precision> <recall>";
        for( uint i=0; i<class_mapping.size(); ++i) {
            const cluster_info_t& ci = class_mapping[i];
            const string& cluster_name   = std::get<0>(ci);
            const uint true_positives    = std::get<1>(ci);
            const uint false_positives   = std::get<2>(ci);
            const uint false_negatives   = std::get<3>(ci);
            const real precision         = std::get<4>(ci);
            const real recall            = std::get<5>(ci);

            avg_true_positives  += true_positives;
            avg_false_positives += false_positives;
            avg_false_negatives += false_negatives;
            avg_precision       += precision;
            avg_recall          += recall;

            LOG(info) << cluster_name << " " << true_positives << " " << false_positives << " " << false_negatives << " " << precision << " " << recall;
        }

        avg_true_positives  /= class_mapping.size();
        avg_false_positives /= class_mapping.size();
        avg_false_negatives /= class_mapping.size();
        avg_precision       /= class_mapping.size();
        avg_recall          /= class_mapping.size();

        LOG(info) << "Average: <true positives> <false positives> <false negatives> <precision> <recall>";
        LOG(info) << avg_true_positives << " " << avg_false_positives << " " << avg_false_negatives << " " << avg_precision << " " << avg_recall;


        LOG(info) << "Writing stats to file \"" << params.precision_recall_file << "\"...";
        to_file( params.precision_recall_file, class_mapping);

    }


    /** Stream out operator for cluster_info_t objects used by the precision/recall tests.
     * @param[in,out] stream A stream that will be modified.
     * @param ci The cluster_info_t object that is to be written to the stream.
     * @return Reference to the exact given stream.
     */
    std::ostream& operator<< (std::ostream& stream, const cluster_info_t& ci) {
        const string& cluster_name   = std::get<0>(ci);
        const uint true_positives    = std::get<1>(ci);
        const uint false_positives   = std::get<2>(ci);
        const uint false_negatives   = std::get<3>(ci);
        const real precision         = std::get<4>(ci);
        const real recall            = std::get<5>(ci);

        stream << cluster_name << " " << true_positives << " " << false_positives << " " << false_negatives << " " << precision << " " << recall;
        return stream;
    }

} // END namespace app
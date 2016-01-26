/******************************************************************************
/* @file Starting point of the Clusterer application.
/*
/* @author langenhagen
/* @version 150120
/******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <program_options.hpp>
#include <input_request.hpp>
#include <clusterer/KMeansClusterer.hpp>
#include <clusterer/OutlierClusterer.hpp>
#include <clusterer/OPTICSClusterer.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <locale>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS
using namespace app;
using namespace std;


Clusterer* create_clusterer( clusterer_description& description);

template< typename T> vector<vector<T>> segment( const vector<uint>& index_mappings,
                                                 const int n_segments,
                                                 const vector<T>& values);

template< template <typename T, typename = std::allocator<T>> class container_type>

std::vector<std::string> create_paths( const std::string& directory,
                                       const container_type<std::string>& file_names,
                                       const std::string& suffix);

bool batch_symlink( const string& output_directory, 
                    const Vec1str& folder_names, 
                    const vector<Vec1str>& segmented_fnames);


/// The program's main enry point.
int main(int argc, const char* argv[]) {
    setlocale(LC_ALL, "");
	cout << "   ***********************************************\n"
            "   *** Welcome to Andis Masterarbeit Clusterer ***\n" 
            "   ***********************************************\n"
	        "\n";
    program_options params;      // stores program options
    
    // init basis modules & log allowed keycodes & parameters
    exit_if_false( init_program_options( argc, argv, params), RETURN_CODE::PROGRAM_OPTIONS_ERROR);
    logging::init_log(params.log_file);
    exit_if_false( adjust_program_options( params), RETURN_CODE::PROGRAM_OPTIONS_ERROR);
    log_program_options(params);
    log_clusterer_types();
    LOG(info) << "*** Program start ***";

    if( boost::filesystem::exists(params.output_directory)) {
        LOG(info) << "Deleting old output files...";
        remove_path(params.output_directory);
    }
    create_directories( params.output_directory);
    
    
    LOG(info) << "Loading feature vectors...";
    Mat1r features;
    exit_if_false( from_file( params.features_file, features), RETURN_CODE::IO_ERROR);
    LOG(info) << "Loading image filenames...";
    Vec1str img_fnames;
    exit_if_false( from_file( params.images_file, img_fnames), RETURN_CODE::IO_ERROR);
    if( features.rows !=  img_fnames.size()) {
        LOG(error) << "The number of features (" << features.rows << ") does not match the number of image filenames (" << img_fnames.size() << ").";
    }

    // *** all clear up to here... features matrix created and valid ***
    
    LOG(info) << "Clustering " << features.rows << " feature vectors with " << features.cols << " dimensions each...";
    Clusterer* clusterer = create_clusterer( params.cd);

    chrono::steady_clock::time_point timer_start = chrono::steady_clock::now();
    
    const Mat1r membership_probabilities = clusterer->cluster( features);
    assert( features.rows == membership_probabilities.rows && "Clusterer must assign probabilities to all input features");

    const timespan duration = chrono::round<timespan>(chrono::steady_clock::now() - timer_start);
    LOG(info) << "Clustering finished. Took " << duration << ".";
    
    const uint n_clusters = membership_probabilities.cols;
    const vector<uint> membership_mappings = Clusterer::assign( membership_probabilities);
    Vec1str cluster_names;
    for( uint i=0; i<n_clusters; ++i)
        // create cluster names
        cluster_names.push_back( params.result_file_prefix + to_string((long long)i));
    LOG(info) << "Found " << n_clusters << (n_clusters == 1 ? " cluster" : " clusters");

    // file outputs
    Vec1str cluster_filenames = create_paths( params.output_directory, cluster_names, ".txt");
    
    LOG(info) << "Writing membership probabilities to \"" << params.membership_probabilities_file << "\"...";
    to_file( params.membership_probabilities_file, membership_probabilities);
    
    LOG(info) << "Writing cluster membership mappings to \"" << params.membership_mappings_file << "\"...";
    to_file( params.membership_mappings_file, membership_mappings);
    

    LOG(info) << "Calculating cluster means...";
    Mat1r cluster_means = Mat1r::zeros( n_clusters, features.cols);
    vector<uint> n_elems_per_cluster( n_clusters, 0);
    for( int i=0; i<features.rows; ++i) {
        
        const uint c = membership_mappings[i];
        cluster_means.row(c) += features.row(i);
        n_elems_per_cluster[c] += 1;
    }
    for( uint i=0; i<n_clusters; ++i) {
        cluster_means.row(i) /= n_elems_per_cluster[i];
    }


    LOG(info) << "Writing Cluster means to \"" << params.cluster_means_file << "\"...";
    to_file( params.cluster_means_file, cluster_means);
    
    LOG(info) << "Writing clusters to files...";
    const vector<Vec1str> segmented_img_fnames = 
        segment( membership_mappings,  n_clusters, img_fnames);
    
    for( uint i=0; i<segmented_img_fnames.size(); ++i)
        to_file( cluster_filenames[i], segmented_img_fnames[i]);
    LOG(info) << "Writing paths of all cluster files to \"" << params.result_fnames_file << "\"...";
    to_file( params.result_fnames_file, cluster_filenames);

    
    // symlink images and additional stuff
    
    if (params.symlink_results) {
        LOG(info) << "Symlinking images into \"" << params.output_directory << "\"...";
        batch_symlink( params.output_directory, cluster_names, segmented_img_fnames);
    }

    if (params.symlink_saliency_maps) {
        // for investigative purposes
        LOG(info) << "Symlinking saliency maps into \"" << params.output_directory << "\"...";
        Vec1str saliency_map_fnames;
        from_file( params.saliency_maps_file, saliency_map_fnames);
        const vector<Vec1str> segmented_saliency_map_fnames = 
            segment( membership_mappings, n_clusters, saliency_map_fnames);
        batch_symlink( params.output_directory, cluster_names, segmented_saliency_map_fnames);
    }


    RELEASE(clusterer);
    EXIT(0);
}


// HELPER FUNCTIONS ///////////////////////////////////////////////////////////


/** Given its description, creates an appropriate clusterer.
 * If the given type is not supported, logs an error message and returns 0.
 * @param description. A filled clusterer_description.
 * @return A clusterer of the type specified in the description.
 *         Returns nullptr, if the given type is not supported.
 */
Clusterer* create_clusterer( clusterer_description& description) {
    Clusterer* ret(nullptr);
    switch( description.type) {
    case clusterer_type::FLANNKMEANS:
        ret = new KMeansClusterer( description);
        break;
    case clusterer_type::OUTLIER:
        ret = new OutlierClusterer( description);
        break;
    case clusterer_type::OPTICS:
        ret = new OPTICSClusterer( description);
        break;
    default:
        LOG(error) << "Unsupported clusterer_type: " << description.type << " aka " << description.type_string << ".";
    }
    return ret;
}


/** Applies the segmentation found with the given indices to the given un-clustered values.
 * @param index_mappings Maps the values to the indidex. Values withing that variable must not exceed n_segments.
 * @param n_segments The number of segments.
 * @param values A container consisiting of all values which are to be segmented into the given order of the indices.
 * @return Returns vectors of segmented values.
 *
 * XXX make more generic
 */
template< typename T>
vector<vector<T>> segment( const vector<uint>& index_mappings,
                           const int n_segments,
                           const vector<T>& values) {
    assert( index_mappings.size() == values.size() && "vectors must have same size");
    vector<vector<T>> ret;
    ret.reserve( n_segments);
    ret.resize( n_segments);

    for( uint i=0; i<values.size(); ++i)
        ret[index_mappings[i]].push_back( values[i]); 

    return ret;
}


/** Creates absolute file paths for given file_names and a given root directory.
 * @param directory The root directory of the files path.
 * @param file_names The names of each file without suffix in an stl compliant iterable container like vectors or queues.
 * @param suffix The filename extension of each file, e.g. ".txt".
 * @return A vector with strings containing absolute paths to the files.
 */
template< template <typename T, typename = std::allocator<T>> class container_type>
std::vector<std::string> create_paths( const std::string& directory, const container_type<std::string>& file_names, const std::string& suffix=".txt" ) {
    std::vector<std::string> ret;

    for( auto it=file_names.begin(); it!=file_names.end(); ++it) {
        std::stringstream ss;
        ss << directory << "/" << *it << suffix;
        bfs::path p( ss.str());
        p = bfs::absolute(p);
        p.make_preferred();
        ret.push_back( p.string());
    }
    return ret;
}


/** Symlinks the results into subfolders that represent clusters in the given output directory. The subfolders 
 * will be created and will be named according their given names.
 * The function returns if any error occurs.
 * @param output_directory The directory in which the subfolders are to be created.
 *        If the directory doesn't exist, it will be created.
 * @param folder_names The filename of every folder.
 * @clusters segmented_fnames Containers with segmented paths.
 *           The length of this container should be equal or bigger than the number of the given folder names.
 * @return TRUE in case of full success,
 *         FALSE in case of any filesystem related error or exception.
 */
bool batch_symlink( const string& output_directory, const Vec1str& folder_names, const vector<Vec1str>& segmented_fnames) {
    assert( folder_names.size() <= segmented_fnames.size() && "vectors must have same size");
    bool ret(true);

    for( uint i=0; i<segmented_fnames.size(); ++i) {
        stringstream ss;
        ss << output_directory << "/" << folder_names[i];
        bfs::path cluster_folder(ss.str());
        cluster_folder.make_preferred();

        if( !app::create_directories(cluster_folder)) {
            ret = false;
            break;
        }

        for(auto it = segmented_fnames[i].begin(); it!=segmented_fnames[i].end(); ++it) {
            bfs::path original_path = bfs::path( *it);
            stringstream symlink_stream;
            symlink_stream << cluster_folder.string() << "/" << original_path.filename().string();
            bfs::path symlink_path( symlink_stream.str());
            symlink_path.make_preferred();

            ret = symlink( original_path, symlink_path);
            if(ret==false)
                break;
        }
        if(ret==false)
            break;
    }
    return ret;
}
/******************************************************************************
/* @file Starting point of the Feature Generator application.
/* 
/* @author langenhagen
/* @version 150120
/******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <program_options.hpp>
#include <input_request.hpp>
#include <ImageProcessor.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <locale>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS
using namespace app;
using namespace std;


bool parse_directories_file( const string& fname, Vec1str& out_directories);
int process_file( const boost::filesystem::path& image_path, ImageProcessor& image_processor);
unordered_set<string> get_already_processed_images( string& fname);
void pause_mode( const global_stats& stats);

/// The program's main enry point
int main(int argc, const char* argv[]) {
    setlocale(LC_ALL, "");
	cout << "   *******************************************************\n"
            "   *** Welcome to Andis Masterarbeit Feature Generator ***\n" 
            "   *******************************************************\n"
	        "\n";
    program_options params;         // stores program options
    global_stats stats = {          // stores global statistics
        0 /*n_processed_images*/,
        0 /*n_processed_images_in_current_session*/,
        0 /*n_images_without_salient_regions*/,
        chrono::steady_clock::now() /*app_starting_time*/,
        timespan() /*summed_processing_timespan*/
    };

    // init basis modules & log allowed keycodes & parameters
    exit_if_false( init_program_options( argc, argv, params), RETURN_CODE::PROGRAM_OPTIONS_ERROR);
    logging::init_log(params.log_file);
    exit_if_false( adjust_program_options( params), RETURN_CODE::PROGRAM_OPTIONS_ERROR);
    log_program_options(params);
    log_keyboard_commands();
    log_extractor_types();
    LOG(info) << "*** Program start ***";

    // The image processor that does the cv related work
    ImageProcessor image_processor( params, stats);
   
    // if specified, delete old features
    if( params.delete_old_features) {
        LOG(info) << "Deleting old file contents and old files...";
        delete_file_contents( params.features_file);
        delete_file_contents( params.processed_images_file);
        delete_file_contents( params.garbage_file);
        delete_file_contents( params.saliency_maps_file);
        delete_file_contents( params.saliency_masks_file);
        remove_path( params.output_directory);
    }

    // create output directories
    app::create_directories( params.output_directory);
    
    LOG(info) << "Parsing directories file...";
    Vec1str& img_dirs = params.image_directories;
    exit_if_false( parse_directories_file( params.directories_file, img_dirs), RETURN_CODE::IO_ERROR);
    LOG(info) << img_dirs.size() << (img_dirs.size() == 1 ? " directory" : " directories") << " to be parsed:\n[" << to_string<string, vector>(img_dirs, "\n") << "].";

    LOG(info) << "Reading already processed images from file...";
    unordered_set<string> already_processed_images;
    getlines_from_file( params.processed_images_file, 
                        [&already_processed_images](string& line, unsigned int line_nr){ already_processed_images.insert(line); } );
    LOG(info) << "Reading further already processed images from \"no salient regions\"-file...";
    getlines_from_file( params.garbage_file, 
                        [&already_processed_images](string& line, unsigned int line_nr){ already_processed_images.insert(line); } );

    LOG(info) << already_processed_images.size() << (already_processed_images.size() == 1 ? " image" : " images") << " already processed.";
    stats.n_processed_images = (uint)already_processed_images.size();

    // check all given directories for images and process each image
    input_request::input_request keyboard_input(input_request::NONE);
    for( auto it = img_dirs.begin(); it != img_dirs.end(); ++it) {
        // for all directory-entries in list
        for( auto dir = bfs::recursive_directory_iterator(*it); dir != bfs::recursive_directory_iterator(); ++dir) {
            if( params.include_subdirs || dir.level()==0) {
                // for each file / subdirectory in entry
                auto already_processed_it = already_processed_images.find( dir->path().string());
                if ( already_processed_it != already_processed_images.end() ) {
                    already_processed_images.erase(already_processed_it);
                } else {
                    // *** file was not processed before ***
                    process_file( bfs::path(*dir).make_preferred(), image_processor);

                }
                keyboard_input = get_keyboard_input();
                if( keyboard_input == input_request::EXIT)
                    break;
                else if( keyboard_input == input_request::PAUSE) {
                    pause_mode( stats);
                    continue;
                }
            }
        }
        if( keyboard_input == input_request::EXIT)
            break;
    }
    // *** shutdown requested or work finished ***

    if( keyboard_input == input_request::EXIT) {
        LOG(notify) << "Shutting down due to keyboard exit request.";
    } else {
        LOG(notify) << "Processing finished.";
    }

    log(stats);

    EXIT(0);
}


// HELPER FUNCTIONS ///////////////////////////////////////////////////////////


/** Parses a file containing directories separated by newlines.
 * @param fname The name of the file to be parsed.
 * @param[out] out_directories The output of the function: A List of the directories that are parsed.
 * @return Returns TRUE in case of success, returns FALSE in case of failure.
 */
bool parse_directories_file( const string& fname, Vec1str& out_directories) {
    bool ret = getlines_from_file( 
        fname,
        [&out_directories](string& dir, unsigned int line_nr) {
            if( !bfs::exists( dir)) {
                LOG(warn) << FILE_LINE << "parse_directories_file(): \"" << dir << "\" does not exist. Continuing gratefully.";
            } else if( !bfs::is_directory( dir)) {
                LOG(warn) << FILE_LINE << "parse_directories_file(): \"" << dir << "\" is not a directory. Continuing gratefully.";
            } else {
                // *** dir is a valid directory ***
                out_directories.push_back( dir);
            }
        });

    if( out_directories.size() == 0) {
        LOG(notify) << "There is no image database specified. You maybe want to fill the \"" << fname << "\" file with valid directory paths.";
        ret = false;
    }
    return ret;
}


/** Puts a file into a processing chain and lets it being processed.
 * @param image_path A path to an image file.
 * @param image_processor The ImageProcessor to be used.
 * @return Returns 0 in case of success,
 * returns 1 if the given directory entry is either a folder or has
 * a not supported extension (see implementation details).
 */
int process_file( const bfs::path& image_path, ImageProcessor& image_processor) {
    int ret(0);

    if ( !is_image_filetype_supported( image_path.extension().string())) {
        ret = 1;    
    } else {
        // *** dir entry has a supported file extension ***
        const string fname = image_path.string();
        Mat3b image = cv::imread( fname, CV_LOAD_IMAGE_COLOR);
        if( image.data == 0) {
            LOG(error) << FILE_LINE << " process_file(): Image \"" << fname << "\" could not be read, either due to improper permissions,"
                       "invalid file format or because of missing file.";
            ret = 2;
        } else {
            // *** all clear up to here. processing chain for each image begins now ***
            chrono::steady_clock::time_point timer_start;
            timespan duration;

            LOG(info) << "Processing \"" << fname << "\"...";
            timer_start = chrono::steady_clock::now();
            
            try {
                ret = image_processor.process_image( image_path, image);
            } catch( const std::exception& e) {
                LOG(app::exception) << "Unhandled exception:\n" << 
                                       e.what();
            } catch ( ...) {
                LOG(app::exception) << "Unhandled exception: exception unknown";
            }


            duration = chrono::round<timespan>(chrono::steady_clock::now() - timer_start);
            image_processor.stats.summed_processing_timespan+= duration;
            LOG(info) << "Finished processing, took " << duration << ".";
        }
    }
    return ret;
}


/** Sets the application in a pause mode that logs statistics
 * halts the image processing and logs the global stats.
 * @param stats The global statistics to log.
 */
void pause_mode( const global_stats& stats) {
    LOG(notify) << " *** PAUSE ***"
                   "\n";
    log(stats);
    LOG(notify) << " *** PRESS ANY KEY TO PROCEED ***";

    cin.sync();
    getch();
}
/******************************************************************************
/* @file Contains the class holding the whole logic for processing one image.
/*
/* @author langenhagen
/* @version 150601
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <program_options.hpp>
#include <global_stats.hpp>
#include <saliency/SaliencyFilters.hpp>
#include <extractor/HistogramExtractor.hpp>
#include <extractor/ContourExtractor.hpp>
#include <extractor/ContourHistogramExtractor.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <opencv2/highgui/highgui.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /** @brief Contains the whole stuff needed for processing an image.
     * It chooses the appropriate saliency detector and feature extractor
     * and contains the processing chain within one function call.
     * It also handles the writing of intermediate results to disk.
     * The object maintains
     *   (1) The saliency map generator.
     *   (2) Creates salient object FeatureExtractor.
     */
    class ImageProcessor {

    public: // vars

        program_options& params; ///< The program parameters.
        global_stats& stats;     ///< Global stats reference.

    private: // vars

        /// The saliency detector.
        SaliencyDetector* _saliency_detector;
        /// The feature extractor.
        FeatureExtractor* _feature_extractor;

        /// Feature vector file stream. Remains open for the whole lifetime of the ProcessingChain object.
        std::ofstream _features_fstream;
        /// File of already processed images stream. Remains open for the whole lifetime of the ProcessingChain object.
        std::ofstream _processed_images_fstream;
        /// File of images without detected salient regions. Remains open for the whole lifetime of the ProcessingChain object.
        std::ofstream _garbage_images_fstream;
        /// File of created saliency maps. Remains open for the whole lifetime of the object.
        std::ofstream _saliency_maps_fstream;
        /// File of created saliency masks. Remains open for the whole lifetime of the object.
        std::ofstream _saliency_masks_fstream;

    public: // constructor & destructor

        /** Main constructor.
         * @param p the program parameters that are to be 
         * used for the processing chain.
         * @param s the global stats of the program.
         */
        ImageProcessor( program_options& p, global_stats& s) 
            : params(p),
            stats(s),
            _features_fstream( params.features_file,                  std::ios::out | std::ios::app),
            _processed_images_fstream( params.processed_images_file,  std::ios::out | std::ios::app),
            _garbage_images_fstream(   params.garbage_file,           std::ios::out | std::ios::app),
            _saliency_maps_fstream(    params.saliency_maps_file,     std::ios::out | std::ios::app),
            _saliency_masks_fstream(   params.saliency_masks_file,    std::ios::out | std::ios::app) {
                
                _saliency_detector = new SaliencyFilters(params.sdd);
                
                switch( params.fed.type) {
                case extractor_type::HISTOGRAM:
                    _feature_extractor = new HistogramExtractor( params.fed);
                    break;
                case extractor_type::CONTOUR:
                    _feature_extractor = new ContourExtractor( params.fed);
                    break;
                case extractor_type::CONTOUR_HISTOGRAM:
                    _feature_extractor = new ContourHistogramExtractor( params.fed);
                    break;
                default:
                    LOG(error) << FILE_LINE << "Unsupported feature_type " << params.fed.type << " aka " << params.fed.type_string << "!";
                }

                if(!_features_fstream.is_open() || _features_fstream.bad()) {
                    LOG(error) << "Creating/opening features file \"" << params.features_file << "\" failed!";
                }
                if(!_processed_images_fstream.is_open() || _processed_images_fstream.bad()) {
                    LOG(error) << "Creating/opening processed images file \"" << params.processed_images_file << "\" failed!";
                }
                if(!_garbage_images_fstream.is_open() || _garbage_images_fstream.bad()) {
                    LOG(error) << "Creating/opening garbage file \"" << params.garbage_file << "\" failed!";
                }
                if(!_saliency_maps_fstream.is_open() || _saliency_maps_fstream.bad()) {
                    LOG(error) << "Creating/opening saliency maps file \"" << params.saliency_maps_file << "\" failed!";
                }
                if(!_saliency_masks_fstream.is_open() || _saliency_masks_fstream.bad()) {
                    LOG(error) << "Creating/opening saliency mask file \"" << params.garbage_file << "\" failed!";
                }
        }

        /** Destructor.
         */
        ~ImageProcessor() {
            _features_fstream.close();
            _processed_images_fstream.close();
            _garbage_images_fstream.close();
            _saliency_maps_fstream.close();
            _saliency_masks_fstream.close();

            RELEASE(_saliency_detector);
            RELEASE(_feature_extractor);
        }

    public: // methods

        /** Starts the processing chain for one image file.
         * It calculates the salient region, extracts a salient object feature vector 
         * and stores it on the hard disk.
         * @param image_path The path to the image file.
         * @param image The BGR image to process.
         * @param Returns 0 in case of success, otherwise returns some other number.
         */
        bool process_image( const boost::filesystem::path& image_path, const Mat3b& image) {
            bool ret(true);
            Mat1b saliency_map, saliency_mask;
            vector<Contour> contours;
            Vec1r features;

            try {
                saliency_map = _saliency_detector->saliency(image);
            } catch( std::exception& e) {
                saliency_map = Mat1b::zeros(image.rows, image.cols);
                LOG(exception) << "Failed to extract saliency map!\n" << 
                                  e.what();
            }

            saliency_mask = generate_saliency_mask( image, saliency_map, contours);
            
            if(contours.size() == 0) {
                // *** no salient region found ***
                LOG(notify) << "No salient region found in \"" << image_path.string() << "\".";
                ret = handle_garbage_file( image_path);
            } else {
                // *** salient regions found ***
                return_error_code::return_error_code ec = return_error_code::UNSPECIFIED_ERROR;
                try {
                    ec = _feature_extractor->extract(image, saliency_map, saliency_mask, contours, features);
                } catch( std::exception& e) {
                    LOG(exception) << "Failed to extract feature vector!\n" << 
                                      e.what();
                }
                
                
                // *** error with extracted features ***
                switch( ec) {
                case return_error_code::SUCCESS:
                    // *** features successfully extracted ***

                    _features_fstream << features << "\n";
                    ANXIOUS_FLUSH(_features_fstream)
                     
                    if( _features_fstream.bad()) {
                        LOG(error) << "Failed writing feature vector to file \"" << params.features_file << "\"!";
                        ret = false;
                    }
                    _processed_images_fstream << image_path.string() << "\n";
                    ANXIOUS_FLUSH(_processed_images_fstream)

                    if( _processed_images_fstream.bad()) {
                        LOG(error) << "Failed writing to file \"" << params.processed_images_file << "\"!";
                        ret = false;
                    }

                    // store intermediate results
                    imwrite_if( params.save_saliency_maps,  image_path, saliency_map, "_saliency.png", _saliency_maps_fstream, params.saliency_maps_file, contours.size()>0);
                    imwrite_if( params.save_saliency_masks, image_path, saliency_mask, "_saliency_mask.png", _saliency_masks_fstream, params.saliency_masks_file);

                    stats.n_processed_images++;
                    stats.n_processed_images_in_current_session++;
                    break;
                    
                case return_error_code::INFINITE_NUMBER_ERROR:
                    LOG(error) << "Feature extraction error (infinite number) while processing \"" << image_path.string() << "\". Image is considered garbage.";
                    ret = handle_garbage_file( image_path);
                    break;
                case return_error_code::NAN_ERROR:
                    LOG(error) << "Feature extraction error (NAN) while processing \"" << image_path.string() << "\". Image is considered garbage.";
                    ret = handle_garbage_file( image_path);
                    break;
                case return_error_code::UNSPECIFIED_ERROR:
                default:
                    LOG(error) << "Feature extraction error (unknown error) while processing \"" << image_path.string() << "\". Image is considered garbage.";
                    ret = handle_garbage_file( image_path);
                }
            }
            return ret;
        }

    private: // helpers

        /** Stores an image in the garbage image filestream and symlinks it into the
         * garbage folder, if specified in the objects description.
         * Also changes the objects stats member accordingly.
         * @param image_path The path to the image file.
         * @return TRUE in case of success, FALSE in case of any error.
         */
        bool handle_garbage_file( const boost::filesystem::path& image_path ) {
            bool ret(true);

            _garbage_images_fstream << image_path.string() << "\n";
            ANXIOUS_FLUSH(_garbage_images_fstream)

            if( _garbage_images_fstream.bad()) {
                LOG(error) << "Failed writing to file \"" << params.garbage_file << "\"!";
                ret = false;
            }
            if( params.symlink_garbage_files) {
                std::stringstream symlink_stream;
                symlink_stream << params.output_directory << "/c_garbage/";
                bfs::path garbage_dir_path( symlink_stream.str());
                app::create_directories( garbage_dir_path.make_preferred());

                symlink_stream << image_path.filename().string();
                bfs::path symlink_path( symlink_stream.str());
                app::symlink( image_path, symlink_path.make_preferred());
            }
            stats.n_images_without_salient_regions+=1;

            return ret;
        }


        /** Convenience function for logged conditional storing of images related to one given file.
         * The new file's path will, if stored, be composed of the params.output_directory,
         * the stem of the given file and the last filename part, which is just a suffix plus a file extension.
         * e.g. a call would look like:
         * imwrite_if( true, file, saliency_map, "_saliency_map.png", log_file_fstream, log_file_name).
         * @param condition Whether or not to store the image.
         *        TRUE - store the image.
         *        FALSE - don't store the image.
         * @param p A file's path.
         * @param image An image to store.
         * @param last_fname_path Usually the character sequence that ends the filaname-string, 
         *        e.g. ".txt" or "_yeah.bear".
         * @param log_fstream A file to which the full path of the imwrite-operation will be logged.
         * @param log_fname The path to the log_fstream as a string, just for error handling purposes.
         * @param log_condition If TRUE; logs to the given file, doesn't log, if FALSE.
         * @return returns TRUE in case of success, returns FALSE in case of any error.
         */
        bool imwrite_if( bool condition, const boost::filesystem::path& p, cv::Mat image, const string& last_fname_part, std::ofstream& log_fstream, const string& log_fname, bool log_condition=true) {
            bool ret(false);

            if( condition) {
                std::stringstream ss;
                ss << params.output_directory << "/" << p.stem().string() << last_fname_part;
                LOG(info) << "Writing \"" << ss.str() << "\" to disk...";
                ret = cv::imwrite( ss.str(), image); 

                if( log_condition) {
                    bfs::path absolute_file_path = bfs::absolute( bfs::path(ss.str()));
                    absolute_file_path.make_preferred();
                    log_fstream << absolute_file_path.string() << "\n";
                    ANXIOUS_FLUSH(log_fstream)

                    if( log_fstream.bad()) {
                        LOG(error) << "Failed writing to file \"" << log_fname << "\"!";
                        ret = false;
                    }
                }
            }
            return ret;
        }


        /** Generates a b/w simplified mask from the given saliency map.
         * Also retrieves the contours of the mask.
         * @param image The original image.
         * @param saliency_map A grayscale image.
         * @param[out] o_contours Reference to a variable that shall store the saliency_masks contours.
         * @return Returns a simplified saliency 8 bit Uchar b/w mask.
         */
        Mat1b generate_saliency_mask( const Mat3b& image, const Mat1b& saliency_map, vector<Contour>& o_contours) {
            using namespace cv;
            Mat1b ret;

            if( params.use_grabcut) {
                // Grabcut
                ret = grabcut( image, saliency_map);

            } else {
                // classic treshold
                GaussianBlur( saliency_map, ret, params.blur_kernel_size, 0, 0);
                threshold( ret, ret, 255*params.threshold, 255, CV_THRESH_BINARY);
                //threshold( ret, ret, 255*params.threshold, 255, CV_THRESH_OTSU);
            }

            // *** smoothed top-level contours left ***            
            o_contours = generate_contours( ret);
            ret = Mat::zeros(ret.rows, ret.cols, CV_8UC1);
            drawContours(ret, o_contours, -1, cv::Scalar(255), CV_FILLED);
            return ret;
        }
            

        /** Applies the GrabCut algorithm on the given image and uses
         * the specified saliency map as a probability mask.
         * @param image The image on which GrabCut shall be applied.
         * @param saliency_map The saliency map that serves as a probability mask.
         * @return The GrabCut-mask of the foreground object.
         */
        Mat1b grabcut( const Mat3b& image, const Mat1b& saliency_map) {
            using namespace cv;
            Mat1b ret( saliency_map.size(), cv::GC_BGD);
    
            for( int r=0; r<ret.rows; ++r) {
            for( int c=0; c<ret.cols; ++c) {
                if( saliency_map(r,c) > 255 * params.grabcut_foreground_probability)
                    ret(r,c) = cv::GC_PR_FGD;
            }}

            // GrabCut segmentation
            cv::grabCut( image,                     // input image
                         ret,                       // segmentation mask & result
                         cv::Rect(0,0,1,1),         // rectangle containing foreground 
                         cv::Mat(),cv::Mat(),       // models
                         1,                         // number of iterations
                         cv::GC_INIT_WITH_MASK);    // use rectangle


            cv::compare(ret, cv::GC_PR_FGD , ret, cv::CMP_EQ);
            return ret;
        }
            

        /** Generates top level contours from the saliency mask and the values specified 
         * in the member variable params.
         * @param saliency_mask The saliency mask from which to derive the contours.
         * @return A vector of all top-level contours of a sufficient size.
         */
        vector<Contour> generate_contours( const Mat1b& saliency_mask) {
            using namespace cv;
            vector<Contour> ret;
            
            vector<Vec4i> hierarchy;
            findContours( saliency_mask, ret, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
            for( int i=static_cast<int>(ret.size()-1); i >=0; --i) {
                // just keep top-level contours of sufficient size
                if( parent(hierarchy, i) != -1 || contourArea( ret[i]) < params.min_salient_region_size) {
                    ret.erase( ret.begin() + i);
                }
            }
            return ret;
        }
    };

}
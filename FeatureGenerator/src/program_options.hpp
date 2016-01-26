/******************************************************************************
/* @file Program descriptions and command and config file parsing logic.
/* 
/* uses: 
/*          - boost.program_options    command-line and config file parsing
/*
/* @author langenhagen
/* @version 1500611
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <detector_type.hpp>
#include <extractor_type.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <iostream>
#include <stdarg.h> // va_list
#include <string>

#include <boost/program_options.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Holds the attributes for the saliency detector.
    struct saliency_detector_description {
        string type_string;
        detector_type::detector_type type;
        string tweak_vector_string;
        Vec1r tweak_vector; ///< not further specified, may be used by the concrete saliency detector implementations.
    };


    /// Holds the feature extractor factory attributes.
    struct feature_extractor_description {
        string type_string;
        extractor_type::extractor_type type;
        string tweak_vector_string;
        Vec1r tweak_vector; ///< not further specified, may be used by the concrete extractor implementations.
    };


    /** @brief Hold global program configurations. 
     * Holds the global program attributes and the
     * descriptions of the application's modules.
     */
    struct program_options {
        string config_file;
        string log_file;
        
        string directories_file;
        string output_directory;
        string features_file;
        string processed_images_file;
        string garbage_file;
        bool delete_old_features;

        Vec1str image_directories;
        bool include_subdirs;
        
        uint blur_kernel_size_uint;
        cv::Size blur_kernel_size;  ///< adjusted blur_kernel_size_uint.
        real threshold;
        real min_salient_region_size;
        bool use_grabcut;
        real grabcut_foreground_probability;

        bool save_saliency_maps;
        string saliency_maps_file;
        bool save_saliency_masks;
        string saliency_masks_file;
        bool symlink_garbage_files;
        
        saliency_detector_description sdd;
        feature_extractor_description fed;
    };


    /** Logs the given program parameters.
     * @param p The program_options to be logged.
     */
    void log_program_options( const program_options& p) {
        LOG(info) << "*** program parameters ***";
        LOG(info) << "Config file: " << p.config_file;
        LOG(info) << "Log file: " << p.log_file;
        LOG(info) << "Directories file: " << p.directories_file;
        LOG(info) << "Output directory: " << p.output_directory;
        LOG(info) << "Include subdirectories: " << yes_no(p.include_subdirs);
        LOG(info) << "Featuer vector file: " << p.features_file;
        LOG(info) << "Processed files file: " << p.processed_images_file;
        LOG(info) << "\"No saliency found in\"-file: " << p.garbage_file;
        LOG(info) << "Discard feature vectors from previous run: " << yes_no(p.delete_old_features);
        LOG(info) << "Saliency feature mask smoothness filter-size: " << p.blur_kernel_size_uint << "px";
        LOG(info) << "Saliency feature cutout threshold: " << p.threshold;
        LOG(info) << "Minimum salient region size: " << p.min_salient_region_size << "px";
        LOG(info) << "Use GrabCut postprocessing: " << yes_no(p.use_grabcut);
        LOG(info) << "GrabCut foreground threshold: " << p.grabcut_foreground_probability;
        LOG(info) << "Save saliency maps: " << yes_no(p.save_saliency_maps);
        LOG(info) << "Saliency maps file: " << p.saliency_maps_file;
        LOG(info) << "Save saliency masks: " << yes_no( p.save_saliency_masks);
        LOG(info) << "Saliency masks file: " << p.saliency_masks_file;
        LOG(info) << "Symlink garbage files: " << yes_no( p.symlink_garbage_files);
        LOG(info) << "Saliency detector type: " << p.sdd.type << " aka " << p.sdd.type_string;
        LOG(info) << "Saliency detector tweak vector: [" << to_string( p.sdd.tweak_vector) << "]";
        LOG(info) << "Feature extractor type: " << p.fed.type << " aka " << p.fed.type_string;
        LOG(info) << "Feature extractor tweak vector: [" << to_string( p.fed.tweak_vector) << "]";
    }


    /** Prints a help message consisting of all given opptions_description pointers.
     * @param args The number of given options_description pointers.
     * @param ... an arbitrary number of options_description pointers.
     */
    void print_help_message( const int args, ... ) {
        using namespace boost::program_options; // options_description
        va_list vl;
        va_start(vl,args);

        for( int i=0; i< args; ++i) {
            options_description* d = va_arg(vl,options_description*);
            std::cout << *d << "\n";
        }
        va_end(vl);
    }


    /** Handles wrong input and either sets it right or writes error messages.
     * Decoupled from init_program_options for readability and the chance to init
     * stuff in between original init and adjustment.
     * @param[in,out] p The proram parameters that are to be checked for correctness.
     * @return TRUE - everything correct.
     *         FALSE - uncorrectable input.
     */
    bool adjust_program_options( program_options& p) {
        bool ret(true);

        if( p.blur_kernel_size_uint % 2 == 0) {
            LOG(warn) << "Given blur_kernel_size must be an odd number! Adjusting to " << ++p.blur_kernel_size_uint;
        }
        p.blur_kernel_size = cv::Size( p.blur_kernel_size_uint, p.blur_kernel_size_uint);

        p.sdd.type = detector_type_from_string( p.sdd.type_string);
        if( p.sdd.type == detector_type::ERROR_TYPE) 
            ret = false;
        p.sdd.tweak_vector = from_string<real,vector>( p.sdd.tweak_vector_string);

        p.fed.type = extractor_from_string( p.fed.type_string);
        if( p.fed.type == extractor_type::ERROR_TYPE)
            ret = false;
        p.fed.tweak_vector = from_string<real,vector>( p.fed.tweak_vector_string);

        if(ret==false) {
            LOG( error) << "Not correctable error in program options!";
        }
        return ret;
    }


    /** Initializes the program options.
     * @param argc The argument counter (e.g. coming from the main function).
     * @param argv The argument vector array (e.g. coming from the main function).
     * @param[out] p The program parameters that are to be filled.
     * @return Returns TRUE in case of success,
     * returns FALSE in case of failure.
     */
    bool init_program_options( const int argc, const char* argv[], program_options& p) {
        using namespace boost::program_options;
        using namespace std;
        bool ret(true);

        // declare the options ************************************************

        // 1. group of options that will be allowed only on command line
        options_description cmd_desc("Command line options options");
        cmd_desc.add_options()
            ("help", "produce help message")
            ("config,c", value<string>(&p.config_file)->default_value("config.cfg"), "name of the config file")
            ;
        // 2. group description on the config file
        options_description config_desc("Configuration file parameters");
        config_desc.add_options()
            ("log_file", value<string>(&p.log_file)->default_value("log.log"), "name of the log file")
            ("directories_file", value<string>(&p.directories_file)->default_value("directories.txt"), "name of the file that stores the image directories")
            ("include_subdirs", value<bool>(&p.include_subdirs)->default_value(1), "whether or not to include subdirectories in looking through the image databases")
            ("output_directory", value<string>(&p.output_directory)->default_value("out"), "the output directory for eventual output-files")
            ("features_file", value<string>(&p.features_file)->default_value("features.desc"), "a file that stores the salient object feature vectors")
            ("processed_images_file", value<string>(&p.processed_images_file)->default_value("processed_files.txt"), "a file that stores the paths of the already processed images")
            ("garbage_file", value<string>(&p.garbage_file)->default_value("files_with_no_salient_regions.txt"), "a file that stores the paths images in which no salient region was found")
            ("delete_old_features", value<bool>(&p.delete_old_features)->default_value(false), "whether or not to delete/reuse the feature vectors generated in a previous run")
            ("blur_kernel_size", value<uint>(&p.blur_kernel_size_uint), "size of the blur filter to smooth the saliency masks in each dimension")
            ("threshold", value<real>(&p.threshold), "the threshold of deciding when a pixel of the saliency map is part of a salient object. Should be within ]0;1[")
            ("min_salient_region_size", value<real>(&p.min_salient_region_size)->default_value(0), "the minimum size of a salient region in pixels")
            ("use_grabcut", value<bool>(&p.use_grabcut)->default_value(false), "Whether or not to use GrabCut for creation of saliency masks")
            ("grabcut_foreground_probability", value<real>(&p.grabcut_foreground_probability)->default_value(static_cast<real>(0.2)), "The probability of a pixel to be long to the foreground")
            ("save_saliency_maps", value<bool>(&p.save_saliency_maps)->default_value(0), "whether or not to save saliency maps to disk")
            ("saliency_maps_file", value<string>(&p.saliency_maps_file)->default_value("saliency_maps.txt"), "stores the paths to eventually created saliency maps")
            ("save_saliency_masks", value<bool>(&p.save_saliency_masks)->default_value(false), "whether or not to save the saliency masks to disk")
            ("saliency_masks_file", value<string>(&p.saliency_masks_file)->default_value("saliency_masks.txt"), "stores the paths to eventually created saliency masks")
            ("symlink_garbage_files", value<bool>(&p.symlink_garbage_files)->default_value(false), "whether or not to symlink the files without salient regions")
            ("detector_type", value<string>(&p.sdd.type_string), detector_types_string().c_str())
            ("detector_tweak_vector", value<string>(&p.sdd.tweak_vector_string)->default_value(""), "real-numeric tweaks for the saliency detector separated by spaces \" \".")
            ("extractor_type", value<string>(&p.fed.type_string), extractor_types_string().c_str())
            ("extractor_tweak_vector", value<string>(&p.fed.tweak_vector_string)->default_value(""), "real-numeric tweaks for the feature extractor separated by spaces \" \".")
            ;
        // END option declarations ********************************************

        // *** parse the command line and the config files ***
        variables_map vm;

        // 1. parse command line ******************************************
        store(parse_command_line(argc, argv, cmd_desc), vm);
        boost::program_options::notify(vm);

        if( vm.count("help")) {
            // special 'help' case
            print_help_message( 2, &cmd_desc, &config_desc);
            return false;
        }
        // 2. parse config file *******************************************
        ifstream in_file(p.config_file.c_str());
        if (!in_file) {
            std::cerr << "Can not open config file: " << p.config_file << "\n"
                         "Did you create a config file or do you want to specify another config file?\n";
            ret = false;
        } else {
            // *** file access works ***
            parsed_options po (parse_config_file(in_file, config_desc, true));
            store( po, vm);
            boost::program_options::notify(vm);
        }
        in_file.close();
        return ret;
    }
}
/******************************************************************************
/* @file Program descriptions and command and config file parsing logic.
/* 
/* uses: 
/*          - boost.program_options    command-line and config file parsing
/*
/* @author langenhagen
/* @version 150203
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <clusterer_type.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <iostream>
#include <stdarg.h> // va_list
#include <string>

#include <boost/program_options.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /// Holds the clusterer attributes.
    struct clusterer_description {
        string type_string;
        clusterer_type::clusterer_type type;

        string tweak_vector_string;
        Vec1r tweak_vector; ///< not further specified, may be used by the concrete clusterer implementations.
    };


    /** Holds the global program attributes and the
     * descriptions of the application's modules.
     */
    struct program_options {
        string config_file;
        string log_file;

        string output_directory;
        string features_file;
        string images_file;

        string membership_probabilities_file;
        string membership_mappings_file;
        string cluster_means_file;
        string result_file_prefix;
        string result_fnames_file;
        bool symlink_results;
        bool symlink_saliency_maps;
        string saliency_maps_file;

        clusterer_description cd;
    };


    /** Logs the given program parameters.
     * @param[in] p The program_options to be logged.
     */
    void log_program_options( const program_options& p) {
        LOG(info) << "*** program parameters ***";
        LOG(info) << "Config file: " << p.config_file;
        LOG(info) << "Log file: " << p.log_file;
        LOG(info) << "Output directory: " << p.output_directory;
        LOG(info) << "Feature vector file: " << p.features_file;
        LOG(info) << "\"path to images\"-file: " << p.images_file;
        LOG(info) << "Clusterer type: " << p.cd.type << " aka " << p.cd.type_string;
        LOG(info) << "Clusterer tweak vector: [" << to_string<real,vector>( p.cd.tweak_vector) << "]";
        LOG(info) << "Membership probabilities file: " << p.membership_probabilities_file;
        LOG(info) << "Membership mappings file: " << p.membership_mappings_file;
        LOG(info) << "Cluster means file: " << p.cluster_means_file;
        LOG(info) << "Result file prefix: " << p.result_file_prefix;
        LOG(info) << "Result file paths file: " << p.result_fnames_file;
        LOG(info) << "Symlink results: " << yes_no( p.symlink_results);
        LOG(info) << "Symlink saliency maps: " << yes_no( p.symlink_saliency_maps);
        LOG(info) << "Saliency maps file: "<< p.saliency_maps_file;
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

        p.cd.type = clusterer_type_from_string( p.cd.type_string);
        if( p.cd.type == clusterer_type::ERROR_TYPE)
            ret = false;

        p.cd.tweak_vector = from_string<real,vector>( p.cd.tweak_vector_string);

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
    bool init_program_options( int argc, const char* argv[], program_options& p) {
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
        // 2. option group description on the config file
        options_description config_desc("Configuration file parameters");
        config_desc.add_options()
            ("log_file", value<string>(&p.log_file)->default_value("log.log"), "name of the log file")
            ("features_file", value<string>(&p.features_file)->default_value("features.txt"), "a file that stores the feature vectors")
            ("images_file", value<string>(&p.images_file), "a file that stores the paths of the images")
            ("clusterer_type", value<string>(&p.cd.type_string), clusterer_types_string().c_str())
            ("clusterer_tweak_vector", value<string>(&p.cd.tweak_vector_string)->default_value(""), "real-numeric tweaks for the feature extractor separated by spaces \" \".")
            ("output_directory", value<string>(&p.output_directory)->default_value("out"), "the output directory for output-files")
            ("membership_probabilities_file", value<string>(&p.membership_probabilities_file)->default_value("membership_probabilities.txt"), "Stores the probabilities of each feature to belong to each cluster")
            ("membership_mappings_file", value<string>(&p.membership_mappings_file)->default_value("membership_mappings.txt"), "Stores the index of the cluster with the highest membership-probability for each feature")
            ("cluster_means_file", value<string>(&p.cluster_means_file)->default_value("cluster_means.txt"), "Stores the centers of each cluster in this file")
            ("result_file_prefix", value<string>(&p.result_file_prefix)->default_value("class_"), "the prefix of the files that contain the classification results")
            ("result_filenames_file", value<string>(&p.result_fnames_file)->default_value("class_files"), "the file that stores the paths to all result files")
            ("symlink_results", value<bool>(&p.symlink_results)->default_value(0), "whether or not to symlink the images into folders named after their classes")
            ("symlink_saliency_maps", value<bool>(&p.symlink_saliency_maps)->default_value(0), "whether or not to symlink the saliency maps that are eventually generated by the FeatureGenerator")
            ("saliency_maps_file", value<string>(&p.saliency_maps_file)->default_value("saliency_maps.txt"), "the file that stores the paths to all result saliency maps.")
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
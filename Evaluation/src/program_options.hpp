/******************************************************************************
/* @file Program descriptions and command and config file parsing logic.
/* 
/* uses: 
/*          - boost.program_options    command-line and config file parsing
/*
/* @author langenhagen
/* @version 150526
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <evaluation_mode.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <iostream>
#include <stdarg.h> // va_list
#include <string>

#include <boost/program_options.hpp>

///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace app {

    /** Holds the global program attributes and the
     * descriptions of the application's modules.
     */
    struct program_options {
        string config_file;
        string log_file;
    
        string mode_string;
        evaluation_mode::evaluation_mode mode;

        string cluster_file_paths_file;
        string membership_probabilities_file;
        string membership_mappings_file;
        string image_db_directory;

        string posteriors_file;
        string precision_recall_file;
    };


    /** Logs the given program parameters.
     * @param p The program_options to be logged.
     */
    void log_program_options( const program_options& p) {
        LOG(info) << "*** program parameters ***";
        LOG(info) << "Config file: " << p.config_file;
        LOG(info) << "Log file: " << p.log_file;
        LOG(info) << "Mode: " << p.mode << " aka " << p.mode_string;
        LOG(info) << "Cluster file paths file: " << p.cluster_file_paths_file;
        LOG(info) << "Membership probabilities file: " << p.membership_probabilities_file;
        LOG(info) << "Membership mappings file: " << p.membership_mappings_file;
        LOG(info) << "Image DB directory: " << p.image_db_directory;
        LOG(info) << "Posteriors file: " << p.posteriors_file;
        LOG(info) << "Precision/recall file: " << p.precision_recall_file;
    }


    /** Prints a help message consisting of all given opptions_description pointers.
     * @param args The number of given options_description pointers.
     * @param ... an arbitrary number of options_description pointers.
     */
    void print_help_message( int args, ... ) {
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
        
        p.mode = evaluation_mode_from_string( p.mode_string);
        if( p.mode == evaluation_mode::ERROR_MODE)
            ret = false;

        return ret;
    }


    /** Initializes the program options.
     * @param argc The argument counter (e.g. coming from the main function).
     * @param argv The argument vector array (e.g. coming from the main function).
     * @param p The program parameters that are to be filled.
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
        // 2. group description on the config file
        options_description config_desc("Configuration file parameters");
        config_desc.add_options()
            ("log_file", value<string>(&p.log_file)->default_value("log.log"), "name of the log file")
            ("cluster_file_paths_file", value<string>( &p.cluster_file_paths_file), "the file containing the paths to the cluster files")
            ("mode", value<string>(&p.mode_string), evaluation_modes_string().c_str())
            ("membership_probabilities_file", value<string>(&p.membership_probabilities_file), "the file containing the row-wise membership probabilites")
            ("membership_mappings_file", value<string>(&p.membership_mappings_file), "the file containing the row-wise membership mappings")
            ("image_db_directory", value<string>(&p.image_db_directory), "the directory of the image database")
            ("posteriors_file", value<string>(&p.posteriors_file)->default_value("posteriors.txt"), "the file that will store the posterior probabilities")
            ("precision_recall_file", value<string>(&p.precision_recall_file)->default_value("precision_recall.txt"), "the file that will store the output of the precision/recall tests")
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

} // END namespace app
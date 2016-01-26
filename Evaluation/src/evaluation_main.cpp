/******************************************************************************
/* @file Starting point of the Evaluation application.
/*
/*
/* @author langenhagen
/* @version 150526
/******************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <precision_recall_test.hpp>
#include <bayes_test.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)


///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS
using namespace app;
using namespace std;


/// The program's main enry point.
int main(int argc, const char* argv[]) {
    setlocale(LC_ALL, "");
	cout << "   ************************************************************\n"
            "   *** Welcome to Andis Masterarbeit Evaluation Application ***\n" 
            "   ************************************************************\n"
	        "\n";
    program_options params;      // stores program options

    // init basis modules & log allowed keycodes & parameters
    exit_if_false( init_program_options( argc, argv, params), RETURN_CODE::PROGRAM_OPTIONS_ERROR);
    logging::init_log(params.log_file);
    exit_if_false( adjust_program_options( params), RETURN_CODE::PROGRAM_OPTIONS_ERROR);
    log_program_options(params);
    LOG(info) << "*** Program start ***";

    if( params.mode == evaluation_mode::PRECISION_RECALL) {
        LOG( info) << "Mode: Precision / Recall";
        eval_precision_recall( params);

    } else if( params.mode == evaluation_mode::BAYES_INFERENCE){
        LOG( info) << "Mode: Bayes inference";
        eval_bayes( params);

    } else {
        // should never happen
        assert( params.mode && false && "evaluation_mode not supported");
        LOG(error) << "Evaluation mode " << params.mode << " aka " << params.mode_string << " not supported by Evaluation application.";
    }

    EXIT(0);
}


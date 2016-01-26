/******************************************************************************
/* Contains Everything to set up a severity logger in your application.
/* 
/* uses: 
/*          - boost.log      for logging
/*          - rlutil         for console coloring       
/*
/* TODO could be optimized, aka deepder understanding of boost log.
/*
/* @author barn
/* @version 141008
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <fstream>
#include <rlutil/rlutil.h>

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

///////////////////////////////////////////////////////////////////////////////
// DEFINES and MACROS

/// Logs something.
#define LOG(level) LOG_NO_BEEP(level)

#ifdef _DEBUG
    #define DLOG(level, msg) LOG(level) << "[d] " << msg;
#else
    #define DLOG(level, msg)
#endif


// Logs something without a beep.
#define LOG_NO_BEEP( level) \
    logging::set_log_color( level); \
    BOOST_LOG_SEV( logging::my_global_logger::get(), level)


/// Logs something, also beeps in error, exception and beep_notification cases.
#define LOG_BEEP(level) \
    set_log_color( level); \
    if( level == logging::error || \
        level == logging::exception || \
        level == logging::beep_notification) \
            printf("\a"); \
    BOOST_LOG_SEV( logging::my_global_logger::get(), level)


///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS

namespace logging {

    /// The application's severity levels.
    enum log_level {
        info,
        notify,
        beep_notify,
        warn,
        error,
        exception
    };


    /** Changes the console font colors according to the log_level.
     * @param level the log_level to set.
     */
    void set_log_color( log_level level) {
        // lookup color codes here:
        // http://tapiov.net/rlutil/docs/HTML/files/rlutil-h.html#rlutil.Color_codes

        using namespace rlutil; // setColor, color enum values
    
        switch( level) {
        case info:
            setColor( GREY);  
            break;
        case notify:
        case beep_notify:
            setColor( LIGHTCYAN);
            break;
        case warn:
            setColor( YELLOW);
            break;
        case error:
            setColor( LIGHTRED);
            break;
        case exception:
            setColor( LIGHTMAGENTA);
            break;
        default:
            setColor( GREY);
        }
    }

    /// sets the console color to standard grey.
    inline void reset_console_colors() {
        rlutil::setColor( rlutil::GREY);
    }


    /// stream out operator for severity levels
    template< typename CharT, typename TraitsT >
    inline std::basic_ostream< CharT, TraitsT >& operator<< ( std::basic_ostream< CharT, TraitsT >& os, log_level level) {
    
        static const char* const level_string[] = {
            "info",
            "notification",
            "notification",  // beep_notification
            "warning",
            "error",
            "exception"
        };

        if (static_cast< std::size_t > (level) < (sizeof(level_string) / sizeof(*level_string)) ) {
            os << level_string[level];
        } else {
            os << static_cast< int >(level);
        }

        return os;
    }


    /** Initializes the log. Should be called at startup.
     * @param fname The name of the log-file.
     */
    void init_log( const std::string& fname) {
        using namespace boost;
        using namespace boost::posix_time;
        using namespace boost::log; // keywords
        using namespace boost::log::expressions; // stream, format_date_time, attr, message
        using namespace boost::log::keywords; // format, file_name

        // console logging sink
        boost::log::add_console_log (
            std::clog, 
            keywords::format = stream << "[" << format_date_time< ptime >("TimeStamp", "%H:%M:%S") << "]" <<
                                         //"[" << attr< log_level >("Severity") << "]:" <<
                                         " " << message);

        // file logging sink
        auto file_sink = boost::log::add_file_log (
            keywords::file_name = fname,
            keywords::auto_flush = true,
            //keywords::filter = expr::attr< log_level >("Severity") >= warning,
            keywords::format = stream << "[" << format_date_time< ptime >("TimeStamp", "%y-%m-%d, %H:%M:%S") << "]"
                                         "[" << attr< log_level >("Severity") << "]:" <<
                                         " " << message);

        // add some commonly used attributes, like timestamp
        boost::log::add_common_attributes();
    
        BOOST_LOG_FUNCTION();
    }


    // initializes the global logger
    BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(my_global_logger, boost::log::sources::severity_logger_mt< log_level >)

} // END namespace log
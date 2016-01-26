/******************************************************************************
/* @file Small, common, generic & clarifying utility/convenience/debugging 
/* functions, macros, constants, typedefs and usings.
/* Some of them may not be used in the current build, but were surely used
/* at some point of the development.
/*
/*
/* @author langenhagen150612
/******************************************************************************/
#pragma once

///////////////////////////////////////////////////////////////////////////////
// INCLUDES project headers

#include <logging.hpp>
#include <Gaussian.hpp>
#include <return_error_code.hpp>

///////////////////////////////////////////////////////////////////////////////
//INCLUDES C/C++ standard library (and other external libraries)

#include <algorithm>
#include <conio.h>
#include <fstream>
#include <iterator>
#include <ostream>
#include <unordered_set>
#include <vector>

#include <boost/chrono.hpp>
#include <boost/filesystem.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

///////////////////////////////////////////////////////////////////////////////
// DEFINES and MACROS

/// used by boost filesystem
#define BOOST_FILESYSTEM_NO_DEPRECATED


/// enhanced delete call.
#define RELEASE( pointer)               \
	if(pointer) {                       \
		delete pointer;                 \
		pointer=nullptr;                \
	}

/// a classical rough exit macro for console applications.
#define CONSOLE_EXIT( return_value)     \
	getch();                            \
	exit(return_value);

/// EXIT macro.
#define EXIT(return_value)              \
    LOG(info) << "*** Program end ***"; \
    CONSOLE_EXIT(return_value)


/// Can be used in combination to print a formatted Filename and line number in a unique fashion.
#define FILE_LINE __FILE__ << ':' << __LINE__ << ": "


/// Can be used to make some critical operations safer (but eventually slower, e.g. file buffer flushing).
#define APP_SAFE


/** Following macros are defined to work in APP_SAFE mode.
 * They may slow down the program but make it more safe or
 * perform a mor in-depth error checking.
 */
#ifdef APP_SAFE

/** Flushes a stream.
 * In developing stage , this makes storing stream data 
 * around exception-throwing areas safe.
 */
#define ANXIOUS_FLUSH( stream)  stream.flush();


/** Performs a validity check of a number.
 * @see validity_check()
 */
#define ANXIOUS_VALIDITY_CHECK(x, alias) check_validity( x, alias, __FILE__, __LINE__);


#else
#define ANXIOUS_FLUSH( stream)
#define ANXIOUS_VALIDITY_CHECK(x)
#endif 
   
///////////////////////////////////////////////////////////////////////////////
// NAMESPACE, CONSTANTS and TYPE DECLARATIONS/IMPLEMENTATIONS


/// Solution namespace.
namespace app {

    namespace bfs = boost::filesystem;
    namespace chrono = boost::chrono;
    
    
    // using log levels for convenient logging
    using logging::info;
    using logging::notify;
    using logging::beep_notify;
    using logging::warn;
    using logging::error;
    using logging::exception;

    using std::string;
    using std::vector;

    using cv::Mat1b;
    using cv::Mat3b;
        
    
    //typedef unsigned char uchar;                    ///< Unsigned char.
    typedef unsigned int uint;                      ///< Unsigned int.
    typedef float real;                             ///< The floating point value. Change at will.
    typedef boost::chrono::milliseconds timespan;   ///< The time granularity to which to round performance counters. Change at will.
    typedef std::vector<real> Vec1r;                ///< A vector of real-values.
    typedef std::vector<uint> Vec1UInt;             ///< A vector of unsigned integer values.
    typedef std::vector<int> Vec1i;                 ///< A vector of integer values.
    typedef std::vector<std::string> Vec1str;       ///< A vector of strings.
    typedef cv::Mat_<real> Mat1r;                   ///< A single channel real valued matrix. Not to be confused with Mat1f or Mat1d!
    typedef cv::Mat_<cv::Vec<real,2>> Mat2r;        ///< A two-channel real valued matrix. Not to be confused with Mat2f or Mat2d!
    typedef cv::Mat_<cv::Vec<real,3>> Mat3r;        ///< A 3 channel real value matrix.
    typedef cv::Point_<real> Point2r;               ///< 2d real valued Point.
    typedef std::vector<cv::Point2i> Contour;       ///< An opencv contour.
    typedef std::unordered_set<uint> UIntSet;       ///< A set of unsigned integers.
    
    typedef Gaussian<app::real> RealGaussian;       ///< Real valued gaussian density function.


    /// Several return error codes.
    namespace RETURN_CODE {
        const int SUCCESS               = 0;
        const int UNDEFINED_ERROR       = 1;
        const int IO_ERROR              = 2;
        const int PROGRAM_OPTIONS_ERROR = 3;
    }


    /** convenience function for non-blocking getch().
     * @return The value that would be returned by getch().
     */
    inline int nb_getch() {
        return kbhit() ? getch() : 0;
    }


    /** Does nothing when the specified value is true, otherwise calls the EXIT macro.
     * Convenience method. Use in conjunction with mission critical functions
     * that return boolean values.
     * @param result If TRUE, nothing happens, if FALSE, EXIT macro will be called.
     * @param on_exit_result The value that will be returned with the EXIT macro.
     */
    inline void exit_if_false( bool result, int on_exit_result = RETURN_CODE::UNDEFINED_ERROR) {
        if( !result) {
            EXIT(on_exit_result);
        }
    }


    /** Open file error callback function originally for getlines_from_file().
     * @see getlines_from_file()
     * @see to_file()
     */
    inline void on_open_file_error( const std::string& fname) {
        LOG(error) << "Opening file \"" << fname << "\" failed.";
    }


    /** Open file error callback function originally for getlines_from_file().
     * @see getlines_from_file()
     */
    inline void on_read_file_error( const std::string& fname, unsigned int line) {
        LOG(error) << "Error while reading file \"" << fname << "\" at line " << line+1 << " (1-based indexing).";
    }


    /** Read line error callback function originally for delete_file_contents().
     * @see delete_file_contents()
     */
    inline void on_delete_file_contents_error( const std::string& fname) {
        LOG(error) << "Deleting contents of \"" << fname << "\" failed!";
    }


    /** Write error callback function originally for to_file().
     * @see to_file()
     */
    inline void on_write_file_error(const std::string& fname) {
        LOG(error) << "Error writing to file \"" << fname << "\".";
    }


    /** filesystem_error exception callback function originally for for_each_file_in_direcory_tree().
     * @see for_each_file_in_direcory_tree()
     */
    void on_filesystem_exception( boost::filesystem::filesystem_error& e) {
        LOG(error) << "Exception with for_each_file_in_direcory_tree(): " << e.what() << "\n";
    }


    /** invalid matrix callback function originally for from_file().
     * @param fname The matrix filename.
     * @param line The line number where the error was created.
     * @see from_file()
     */
    inline void on_invalid_mat( const std::string& fname, const int line) {
        LOG(error) << "Matrix file \"" << fname << "\" invalid at line " << line << ".";
    }


    /** Performs a callback for every directory entry in a directory tree.
     * Also performs exception handling. If a filesystem_exception occurs, 
     * it will be caught and the iteration will be aborted.
     * @param p The root directory path in which to start the iteration.
     * @param callback The callback to perform on every directory entry.
     *        It takes a boost::filesystem::directory_entry as an argument, 
     *        also an unsigned int that will provide the iteration level
     *        and returns a bool. When the return value is FALSE,
     *        the whole iteration will be aborted and the function returns false.
     * @param exception_callback The function that is called when an exception occurs.
     *        Takes a boost::filesystem::filesystem_error as an argument.
     * @param max_level The maximum level to iterate into the directory tree starting where 0 is the base level.
     *        If max_level is set to -1, the function iterates into every file-tree depth.
     * @return TRUE in case of complete success.
     *         FALSE in case of an exception of kind boost::filesystem::filesystem_error.
     */
    bool for_each_file_in_direcory_tree( boost::filesystem::path& p,
                                         std::function<bool( boost::filesystem::directory_entry&, uint level)> callback,
                                         std::function<void( boost::filesystem::filesystem_error&)> exception_callback = on_filesystem_exception,
                                         int max_level = -1) {
        namespace bfs = boost::filesystem;
        bool ret(true);
        try {
            for( bfs::recursive_directory_iterator it(p); it!=bfs::recursive_directory_iterator(); ++it)
                if( max_level == -1 || it.level() <= max_level ) {
                    ret = callback(*it, it.level());
                    if(ret==false)
                        break;
                }
        } catch (bfs::filesystem_error& e) {
            exception_callback(e);
            ret = false;
        }
        return ret;
    }


    /** Convenience function that reads and processes every line in a file.
     * It opens the file, reads, and processes every line via a specified callback and then closes the file.
     * It does proper opening and error handling with help of the specified callback functions for the two error cases.
     * @param fname The path of the file to be read.
     * @param callback The function that is to be called for every line.
     *        It takes a string as an argument which will be set to the line read and an unsigned integer that
     *        reveals the current line number (0-based indexing) to the callback.
     * @param error_open_callback The function that is to be called when the file could not be openend.
     *        It takes a string as an argument which will be set to the original given filename.
     * @param error_read_callback The function that is called when there occured an error while reading the file. 
     *        It takes a string as an argument which will be set to the original given filename.
     * @return TRUE in case of success, 
     *         FALSE in case of error.
     */
    bool getlines_from_file( const std::string& fname,
                             std::function<void(std::string&, unsigned int)> callback,
                             std::function<void(const std::string&)> error_open_callback = on_open_file_error,
                             std::function<void(const std::string&, unsigned int)> error_read_callback = on_read_file_error) {
        bool ret(true);
        std::ifstream in_file( fname);
        if( !in_file.is_open()) {
            error_open_callback( fname);
            ret = false;
        }
        std::string line;
        unsigned int line_nr(0);
        while( getline(in_file, line))
            callback(line, line_nr++);
        if(in_file.bad()) {
            error_read_callback(fname, line_nr);
            ret = false;
        }
        in_file.close();
        return ret;
    }


    /** Convenience function that writes the contents of a container to a specified file.
     * Truncates all old entries in that given file. 
     * Also does error handling via callbacks.
     * @param fname The name of the file to be written to.
     * @param container The stl-compliant container to be written to file.
     * @param delimeter The delimeter between each entry in the container.
     * @param error_open_callback The function that is called when opening the file fails.
     *        It takes a string (the filename) as an argument.
     * @param error_write_callback The function that is called when writing to the file fails.
     *        It takes a string (the filename) as an argument.
     * @return TRUE in case of success, 
     *         FALSE in case of error.
     */
    template< typename F, template<class T, class = std::allocator<T> > class container_type > /* for vectors and stuff */
    bool to_file( const std::string& fname, 
                  const container_type<F>& container,
                  const std::string& delimeter = "\n",
                  std::function< void(const std::string&) > error_open_callback = on_open_file_error,
                  std::function< void(const std::string&) > error_write_callback = on_write_file_error) {
        bool ret(true);
        std::ofstream fstream( fname, std::ios::out | std::ios::trunc);
        if(!fstream.is_open() || fstream.bad()) {
            error_open_callback(fname);
            ret = false;
        } else {
            for( auto it=container.begin(); it!=container.end(); ++it) {
                fstream << *it;
                if( std::next(it) != container.end())
                        fstream << delimeter;

                if( fstream.bad()) {
                    error_write_callback(fname);
                    ret = false;
                    break;
                }
            }
        }
        return ret;
    }


    /** Convenience function that writes the contents of a Mat_ to a specified file.
     * It writes the plain matrix to the file. For more complex data formats consider cv::FileStorage.
     * Truncates all old entries in that given file. 
     * Also does error handling via callbacks.
     * @param fname The name of the file to be written to.
     * @param mat The Mat_ that is to be written to file.
     * @param row_delimeter The delimeter between each row in in the matrix.
     * @param col_delimeter The delimeter between each column in in the matrix.
     * @param error_open_callback The function that is called when opening the file fails.
     *        It takes a string (the filename) as an argument.
     * @param error_write_callback The function that is called when writing to the file fails.
     *        It takes a string (the filename) as an argument.
     * @return TRUE in case of success, 
     *         FALSE in case of error.
     */
    template< class mat_type >
    bool to_file( const std::string& fname, 
                  const mat_type& mat,
                  const std::string& row_delimeter = "\n",
                  const std::string& col_delimeter = " ",
                  std::function< void(const std::string&) > error_open_callback = on_open_file_error,
                  std::function< void(const std::string&) > error_write_callback = on_write_file_error) {
        bool ret(true);
        std::ofstream fstream( fname, std::ios::out | std::ios::trunc);
        if(!fstream.is_open() || fstream.bad()) {
            error_open_callback(fname);
            ret = false;
        } else {
            for( int r=0; r<mat.rows; ++r) {
                for( int c=0; c<mat.cols-1; ++c) {
            
                    fstream << mat(r,c) << col_delimeter;

                    if( fstream.bad()) {
                        error_write_callback(fname);
                        ret = false;
                        break;
                    }
                }
                fstream << mat(r,mat.cols-1);
                if( r < mat.rows-1)
                    fstream << row_delimeter;

                if( fstream.bad()) {
                    error_write_callback(fname);
                    ret = false;
                    break;
                }
            }
        }
        return ret;
    }


    /** Convenience function that appends the given stl compliant container of type string 
     * with the lines found in the given file. In error case it calls the given callbacks.
     * @param fname The path to the file to be read.
     * @param[out] out_container The stl compliant container of in which the read found lines
     *        shall be written. The container must support push_back().
     * @param error_open_callback The function that is to be called when the file could not be openend.
     *        It takes a string as an argument which will be set to the original given filename.
     * @param error_read_callback The function that is called when there occured an error while reading the file. 
     *        It takes a string as an argument which will be set to the original given filename.
     * @return TRUE in case of success, 
     *         FALSE in case of error.
     */
    template< template<class T, class = std::allocator<T> > class container_type > /* for sets and stuff */
    inline bool from_file( const std::string& fname,
                           container_type<std::string>& out_container,
                           std::function<void(const std::string&)> error_open_callback = on_open_file_error,
                           std::function<void(const std::string&, unsigned int)> error_read_callback = on_read_file_error ) {
        
        return getlines_from_file( fname, 
                                   [&out_container](string& line, uint n) { out_container.push_back(line); }, 
                                   error_open_callback,
                                   error_read_callback );
    }


    /** Convenience function that appends the given stl compliant container with the elements
     * found in the lines of the given file. In error case it calls the given callbacks.
     * @param fname The path to the file to be read.
     * @param[out] out_container The stl compliant container of in which the read found lines
     *        shall be written. The container must support push_back().
     * @param error_open_callback The function that is to be called when the file could not be openend.
     *        It takes a string as an argument which will be set to the original given filename.
     * @param error_read_callback The function that is called when there occured an error while reading the file. 
     *        It takes a string as an argument which will be set to the original given filename and an unsigned 
     *        int that represents the error-provoking line in the file (0-based indexing).
     * @return TRUE in case of success, 
     *         FALSE in case of error.
     */
    template< typename F, template<class T, class = std::allocator<T> > class container_type > /* for sets and stuff */
    inline bool from_file( const std::string& fname,
                        container_type<F>& out_container,
                        std::function<void(const std::string&)> error_open_callback = on_open_file_error,
                        std::function<void(const std::string&, unsigned int)> error_read_callback = on_read_file_error ) {
        bool ret(true);

        std::ifstream in_file( fname);
        if( !in_file.is_open()) {
            error_open_callback( fname);
            ret = false;
        } else {
            
            std::string line;
            unsigned int line_nr(0);
            while( getline(in_file, line)) {
                std::stringstream ss;
                ss << line;
                std::copy( std::istream_iterator<F>(ss),
                            std::istream_iterator<F>(),
                            std::back_inserter(out_container));
                line_nr++;
            }
            if(in_file.bad()) {
                error_read_callback(fname, line_nr);
                ret = false;
            }
        }
        in_file.close();
        return ret;
    }


    /** Convenience function that deletes the contents of a specified file and includes error handling.
     * @param fname the path to the file which contents are to be erased.
     * @param error_callback The function that is to be called when opening the file with ios::out | ios::trunc
     *        yields a badbit on the stream. 
     *        It takes a string as an argument which will be set to the original given filename.
     * @return TRUE in case of success,
     *         FALSE in case of error.
     */
    inline bool delete_file_contents( std::string& fname, 
                                      std::function<void(const std::string&)> error_callback = on_delete_file_contents_error) {
        bool ret(true);
        std::ofstream out_file( fname, std::ofstream::out | std::ofstream::trunc);
        if( out_file.bad()) {
            error_callback( fname);
            ret = false;
        }
        out_file.close();
        return ret;
    }

    
    /** Creates a directory and, if necessary, the higher level directories.
     * Logs an error message in case of error.
     * @param p The path of the directory to be created.
     * @return TRUE in case of success,
     *         FALSE in case of error.
     */
    bool create_directories( const boost::filesystem::path& p) {
        bool ret(true);
        boost::system::error_code ec;
        boost::filesystem::create_directories(p, ec);
        if( ec != 0) {
            LOG(error) << "Creating directory \"" << p.string() << "\" failed!\n"
                          "Error code value: " << ec.value() << "\n" 
                          "Error code message; " << ec.message();
            ret = false;
        }
        return ret;
    }


    /** Deletes a path (folder or file) ultimately from the filesystem.
     * Logs error messages in case of some error.
     * @param p A path to a file or a folder.
     * @return TRUE in case of success,
     *         FALSE in case of error.
     */
    bool remove_path( const boost::filesystem::path& p) {
        namespace bfs = boost::filesystem;
        bool ret(true);
        boost::system::error_code ec;

        if( !bfs::exists(p, ec) || ec.value()!=0) {
            LOG(error) << FILE_LINE << "Path " << p << " doesn't exist. Error message:\n" << 
                          ec.message();
            ret = false;
        } else {
            bfs::remove_all(p, ec);
        
            if( ec.value()!=0) {
                LOG(error) << FILE_LINE << "Removing path " << p << " resulted in an error. Error message:\n" << 
                              ec.message();
                ret = false;
            }
        }

        return ret;
    }


    /** Symlinks a file using boost::filesystem::create_symlink. 
     * If an exception is thrown, it will be caught and handled.
     * On Windows 7, this needs elevated rights for your program,
     * check it out on your system.
     * @param original_path The path of the original file.
     * @param symlink_path The path of the symlink to be created.
     * @return TRUE in case of success,
     *         FALSE in case of error.
     */
    bool symlink( const boost::filesystem::path& original_path, const boost::filesystem::path& symlink_path) {
        bool ret(true);
        try {
            // on windows7: works only with elevated rights!
            boost::filesystem::create_symlink( original_path, symlink_path);
        } catch( std::exception& e) {
            LOG(logging::exception) << FILE_LINE << "\n" <<
                                       e.what();
            ret = false;
        }
        return ret;
    }


    /** to-string for stl containers like vectors and stacks.
     * @param The container to be converted to a string.
     * @param The delimeter of the list elements.
     * @return A string consisting of the elements of the container.
     */
    template< typename F, template<class T, class = std::allocator<T> > class container_type >
    inline std::string to_string( container_type<F> container, const char* delimeter = "; ") {
        std::stringstream ss;
        for(auto it = container.begin(); std::next(it)!=container.end(); ++it)
            ss << *it << delimeter;
        ss << container.back();
        return ss.str();
    }


    /** from_stream for stl containers.
     * Puts all elements of the in_stream into a return container.
     * Containers should implement push_back in order to work correctly.
     * @param in_stream The container in an istream form, spaces are supposed to be the delimeters.
     * @return A container consisting of the elements found in the stream.
     */
    template< typename F, template<class T, class = std::allocator<T> > class container_type >
    container_type<F> from_stream( std::istream& in_stream ) {
        container_type<F> ret;
        std::copy( std::istream_iterator<F>(in_stream),
                   std::istream_iterator<F>(),
                   std::back_inserter(ret));
        if( in_stream.fail() && !in_stream.eof()) {
            LOG(error) << FILE_LINE << "Could not parse stream correctly. Maybe stream is falsely formatted.";
        }
        return ret;
    }


    /** Puts all numbers in a given string into an stl container.
     * The container type must be able to perform push_back().
     * @param str The string that is to be converted into a container.
     * @return An stl container of a given numerical type.
     */
    template< typename F, template<class T, class = std::allocator<T> > class container_type >
    inline container_type<F> from_string( const std::string& str) {
        return from_stream<F, container_type>( std::istringstream(str));
    }

    
    /** Loads a (one-dimensional) plain matrix from a file.
     * For more complex data formats consider cv::FileStorage.
     * @param fname The path to the file where the matrix is stored.
     * @param[out] out_mat A reference to the matrix that shall store the matrix.
     * @return TRUE in case of success.
     *         FALSE, if there occured any file i/o related error.
     */
    template< typename T>
    bool from_file( const std::string& fname, cv::Mat_<T>& out_mat,
                    std::function< void(const std::string&, const int line) > error_invalid_mat_callback = on_invalid_mat) {
        bool ret;
        out_mat.release();
        ret = getlines_from_file(
            fname,
            [&]( string& line, uint line_nr) {
                std::vector<T> v = from_string<T,vector>( line);
                
                if( v.size()==0 || out_mat.rows != 0 && v.size() != out_mat.cols) {
                    error_invalid_mat_callback(fname, line_nr);
                    ret = false;
                } else {
                    out_mat.push_back( cv::Mat_<T>(cv::Mat_<T>(v).t())); // not gonna love the MatExpr
                }
        });
        return ret;
    }


    /** Stream out operator on vectors.
     * @param[in,out] os An ostream.
     * @param v a std::vector.
     * @return The ostream that was the first parameter.
     */
    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
        copy(v.begin(), v.end(), ostream_iterator<T>(os, " ")); 
        return os;
    }


    /** Gives a bool into a "yes"/"no" string representation.
     * @param b A boolean value.
     * @return The string representation of the input parameter.
     * true - returns "yes"
     * false - returns "no"
     */
    inline std::string yes_no( bool b) {
        return b ? "yes" : "no";
    }
    

    /** Converts all capital letters in a to lower case.
     * @param s The input string.
     * @return The input string for the sake of inline usage.
     */
    inline std::string& to_lower( std::string& s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }


    /** Copies the given string in a new lower case string.
     * @param s The input string.
     * @return The input string for the sake of inline usage.
     */
    inline std::string to_lower( const std::string& s) {
        std::string ret = s;
        return to_lower( ret);
    }


    /** Retrieves the string giving the Matrix type of an opencv matrix.
     * @param number The numerical matrix type.
     * @return The string representation of the given matrix type.
     */
    std::string get_mat_type(int number) {
        std::stringstream ret;

        std::string img_type_string;
        int img_type_int = number%8;
        switch (img_type_int) {
            case 0:
                img_type_string = "8U";
                break;
            case 1:
                img_type_string = "8S";
                break;
            case 2:
                img_type_string = "16U";
                break;
            case 3:
                img_type_string = "16S";
                break;
            case 4:
                img_type_string = "32S";
                break;
            case 5:
                img_type_string = "32F";
                break;
            case 6:
                img_type_string = "64F";
                break;
            default:
                break;
        }

        int num_channels = (number/8) + 1;
        ret << "CV_" << img_type_string << "C" << num_channels;
        return ret.str();
    }


    /** wraps the value of val into the range [0;upper_limit].
     * You could use it for e.g. wrapping degrees like 721° to a [0°;360°] range.
     * @param val The value to be wrapped.
     * @param upper_limit The upper limit to which to wrap.
     * Values bigger than upper_limit will be wrapped.
     * @return The wrapped value.
     */
    inline double wrap( double val, double upper_limit=360 ) {
        return abs( val - upper_limit * floor( val / upper_limit ));
    }


    /** Retrieves the smallest signed distance between two angles.
     * The result will lie within ]-180°;180°].
     * @param angle 1 An angle.
     * @param angle 2 An angle.
     * @return The smallest signed distance between the two angles.
     */
    inline double angle_diff( double angle1, double angle2) {
         return wrap(angle1 - angle2 + 180, 360) - 180;
    }


    /** A convenience function for better readability of code.
     * equals to hierarchy[contour_index][3];
     * @param hierarchy A hierarchy of contours.
     * @param contour_index The index of a contour.
     * @return The index of the parent contour, -1 if there is no parent-contour.
     */
    inline int parent( std::vector<cv::Vec4i> hierarchy, int contour_index) {
        return hierarchy[contour_index][3];
    }


    /** Resizes a vector and linearly interpolates the vector elements when  upscaling/downscaling.
     * @param v The vector of type T that is to be resized. Type T must understand operations 
     * like addition T+T and multiplication with a floating point value.
     * @param new_size The size of the new output vector.
     * @return An resized, linearly interpolated version of the given vector with the given new length.
     */
    template< typename T>
    std::vector<T> resize_interpolated( const std::vector<T>& v, uint new_size) {
        std::vector<T> ret;
        double factor = double(v.size()) / new_size;

        for( int i=0; i<new_size; ++i) {
            double index = i * factor;
            int integer_part = static_cast<int>(index);
            double fractional_part = index - integer_part;
            if( index+1 >= v.size())
                ret.push_back( v[integer_part]);
            else
                ret.push_back( v[integer_part] * (1 - fractional_part) + v[integer_part + 1] * fractional_part);
        }
        return ret;
    }


    /** Retrieves the normalized circular delayed cross correlatipon series of two given vectors.
     * The function calculates means and deviations under the hood, so maybe, if you have
     * these values beforehand you should rather calculate your own correlation series inline.
     * @param a The first vector.
     * @param b The second vector. Must have the same length as vector a.
     * @return The cross correlation series for both vectors with the index defining the delay.
     * Since cross_correllation(vector, delay) = cross_correllation(vector, vector.size-delay),
     * or, in other words, the cross correlation series is symmetric to delay = vector.size()/2,
     * The resulting vector will be half the size of the input vectors.
     * The vector elements are within [-1;1].
     */
    template<typename R>
    std::vector<R> circular_cross_correlation_series( const std::vector<R>& a, const std::vector<R>& b) {
        assert( a.size() == b.size() && "a and b must have same length");
        std::vector<R> ret;
        unsigned int size = static_cast<unsigned int>(a.size()/2) + (a.size()%2 == 0 ? 0 : 1);
        ret.reserve( size);

        register R mean_a = 0;
        register R mean_b = 0;
        R variance_a = 0;
        R variance_b = 0;
        register R denom;

        // calculate means
        for( uint i=0; i<a.size(); ++i) {
            mean_a += a[i];
            mean_b += b[i];
        }
        mean_a /= a.size();
        mean_b /= a.size();
        // calculate variances
        for( uint i=0; i<a.size(); ++i) {
            variance_a += (a[i] - mean_a) * (a[i] - mean_a);
            variance_b += (b[i] - mean_b) * (b[i] - mean_b);
        }
        denom = sqrt( variance_a * variance_b);
    
        // calculate correlation series
        if( denom == 0) {
            for( unsigned int delay=0; delay<size; ++delay) {
                ret.push_back(1);
            }
        } else {
            for( unsigned int delay=0; delay<size; ++delay) {
                R cross_correlation = 0;
                for( uint idx_a=0; idx_a<a.size(); ++idx_a) {
                    int idx_b = (idx_a+delay) % a.size();
                    cross_correlation += (a[idx_a] - mean_a) * (b[idx_b] - mean_b);
                }
                ret.push_back( cross_correlation / denom);
            }
        }
        return ret;
    }


    /** Retrieves the center of a vector of points.
    * @param v The vector of points.
    * @return The center Point, which is exactly in the middle square 
    * spanned by the leftmost, uppermost, rightmost, lowermost points.
    */
    template< typename T, typename R>
    cv::Point_<R> center( const std::vector<cv::Point_<T>>& v) {
        cv::Point_<R> ret;
        R min_x = v[0].x;
        R min_y = v[0].y;
        R max_x = v[0].x;
        R max_y = v[0].y;

        for(auto it=v.begin(); it!=v.end(); ++it) {
            int x = it->x;
            int y = it->y;

            if( x < min_x)
                min_x = x;
            else if( x > max_x)
                max_x = x;
            if( y < min_y)
                min_y = y;
            else if( y > max_y)
                max_y = y;
        }
        ret.x = (max_x - min_x) / 2 + min_x;
        ret.y = (max_y - min_y) / 2 + min_y;
        return ret;
    }


    /** Checks if a given number is bigger than the type boundaries,
     *  e.g. if a number is infinite..
     *  With C++11 consider using std::isinf.
     * @param v The value to be checked.
     * @return TRUE, if the number is infinite, 
     *        FALSE if the given value is not.
     */
    template<typename T>
    inline bool is_inf( const T &v ) {
        return ! ( -std::numeric_limits<T>::max() <= v && 
                    std::numeric_limits<T>::max() >= v );
    }
 

    /** Checks whether a given number not a number.
     * With C++11 consider using std::isnan.
     * @param v The value to be checked.
     * @return TRUE, if the number is NaN, 
     *        FALSE if the given value is not.
     */
    template<typename T>
    inline bool is_nan( const T &v) {
        return v != v;
    }


    /** Checks wethers a given number is a valid number,
     * e.g. is finite and is not NaN.
     * With C++11 consider using std::isnan and std::isinf.
     * @param v The value to be checked.
     * @return TRUE if the given value is a valid number,
     *         FALSE if it is not.
     */
    template<typename T>
    inline bool is_valid( const T &v) {
        return !(is_inf(v) || is_nan(v));
    }


    /** Check if a given value is infinite or not a number. 
     *  If true, an error message will be logged.
     * @param v The value to check.
     * @param alias The values plain text alias e.g. its human understandable variable name.
     * @param file The name of the file in which the check was to be found.
     * @param line The line number which invoked the validity check.
     * @return TRUE if the number is valid, FALSE if not.
     */
    template<typename T>
    void check_validity( const T& v, const std::string& alias, const char *file, const int line) {
        bool ret(true);
        if( is_inf(v)) {
            LOG(error) << "In " << file << ":" << line << ": Value '" << alias << "' is infinite.";
            ret = false;
        }
        if( is_nan(v)) {
            LOG(error) << "In " << file << ":" << line << ": Value '" << alias << "' is NaN.";
            ret = false;
        }
    }

    /** Checks if a given file extension (with point) is supported by the application.
     * @param extension A file extension with leading point, e.g. ".jpg".
     * @return TRUE - if image filetype is supported.
     *         FALSE otherwise.
     */
    bool is_image_filetype_supported( const string& extension) {
        const int n_supported_filetypes = 14;
        const string supported_filetypes[n_supported_filetypes] = { 
            ".bmp", ".dib", 
            ".jpeg", ".jpg", ".jpe", 
            ".jp2", 
            ".png",
            ".pbm", ".pgm", ".ppm", 
            ".sr", ".ras", 
            ".tiff", ".tif"
        };

        return std::any_of( supported_filetypes,
                             supported_filetypes + n_supported_filetypes, 
                             [&](string filetype){ 
                                return filetype.compare( to_lower( extension )) == 0; 
                             });
    }


} // END namespace app
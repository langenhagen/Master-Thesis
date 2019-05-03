*******************************************************************
README
for the practical solution of the master thesis
"Unsupervised Detection of Salient Regions in Image Databases"
by Andreas Langenhagen

author: langenhagen (barn07@web.de)
date: 2015 07 13
*******************************************************************

CONTENTS:
    0. SOLUTION OVERVIEW
    1. INSTALLATION
    2. USAGE
        2.1 FeatureGenerator
        2.2 Clusterer
        2.3 Evaluation
        2.4 OPTICSAnalyzer
    3. EXTERNAL LIBRARIES
    4. HISTORY



0. SOLUTION OVERVIEW ##############################################################################
###################################################################################################

The Solution holds 5 Projects:

    Common              :       maintains global types, functions, modules etc., no executable
    FeatureGenerator    :       salient region descriptor generator
    Clusterer           :       clusters the images according to the descriptors
    Evaluation          :       assesses the clustering result quality
    OPTICSAnalyzer      :       tool for manual assessment of the OPTICS clustering

    - The thesis document can be found in the root directory under following name:
        "Andreas Langenhagen - Master Thesis - Unsupervised Detection of Salient Regions in Image Databases.pdf"

    - The doxygen documentation can be found in the folder "doc"

    - The Caltech-256 image database can be downloaded from this source:
        http://www.vision.caltech.edu/Image_Datasets/Caltech256/


1. INSTALLATION ###################################################################################
###################################################################################################

The projects in the solution need following external libraries:

    - boost  (tested with version 1.56)
        - chrono
        - filesystem
        - log
        - program_options
    - OpenCV (tested with version 2.49)


In order to link the external libraries to the project, you should specify the following environment variables:

    VARIABLE                    POSSIBLE VALUE

    - BOOST_HOME        :       C:\boost_1_56_0\
    - OPENCV_BUILD      :       C:\opencv-2.49\build\


Eventually you have to copy some missing dlls into the executables directories.



2. USAGE ##########################################################################################
###################################################################################################

The executables can be found in     <SOLUTION-SOLUTION_ROOT>\bin\            and in
                                    <SOLUTION_ROOT>\OPTICSAnalyzer\bin\      , respectively.

2.1 FeatureGenerator ##############################################################################

    The FeatureGenerator takes a config file that is to be filled with the listed keys and values

    INPUT:
    VARIABLE                        EXPLANATION                                                                                 ACCEPTED VALUES
    ----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    log_file                        path to the log file                                                                        path to a file
    directories_file                path to the file that points to the image database directories                              path to a file
    include_subdirs                 whether or not to include sub-directories of the given directories                          {0,1}
    output_directory                output directory for storing saliency maps and –masks on disk                               path to a directory
    delete_old_features             whether or not to delete the results of the last session                                    {0,1}
    features_file                   a file that stores the resulting feature vectors                                            path to a file
    processed_images_file           a file that stores the image-paths that correspond to the feature vectors                   path to a file
    garbage_file                    a file that stores the image-paths which caused technical errors                            path to a file
    detector_type                   the type of salient region detector that is to used                                         { saliency_filters }
    detector_tweak_vector           a vector that parameterizes the detector                                                    vector of real numbers delimited by spaces
    min_salient_region_size         the minimum number of pixels a salient region must contain                                  N+
    blur_kernel_size                size of Gaussian blur kernel that is applied prior to threshold masking the saliency map    {n | n el. N+ , n mod 2 = 1}
    use_grabcut                     whether or not to use GrabCut for saliency mask creation                                    {0,1}
    grabcut_foreground_probability  GrabCut foreground probability                                                              [0..1]
    threshold                       threshold value if simple masking is preferred over GrabCut                                 [0..1]
    extractor_type                  the type of descriptor extractor that is to used                                            { contour, histogram, contour_histogram }
    extractor_tweak_vector          a vector that parameterizes the descriptor extractors                                       vector of real numbers delimited by spaces
    save_saliency_maps              whether or not to store saliency maps on disk                                               {0,1}
    save_saliency_masks             whether or not to store saliency masks on disk                                              {0,1}
    symlink_garbage_files           whether or not to create symbolic links for all files that caused errors                    {0,1}
    saliency_maps_file              a file that points to the stored the saliency maps                                          path to a file
    saliency_masks_file             a file that points to the stored saliency masks                                             path to a file
    ----------------------------------------------------------------------------------------------------------------------------------------------------------------------

    === detector_tweak_vector: ===
    The following tables provide information about the usage of the tweak vectors for the salient region detector modules.

    SALIENCY_FILTERS
    ----------------
        INDEX           EXPLANATION                             ACCEPTED VALUES
         0:             number of superpixels                   N+
         1:             number of k-means iterations for S      N+
         2:             superpixel color weight                 [0..1]
         3:             positional sigma parameter              R
         4:             color sigma parameter                   R
         5:             upsampling parameter k                  R
         6:             minimum saliency percentage             [0..1]
         7:             alpha                                   R
         8:             beta                                    R
         9:             upsamle?                                {0,1}
        10:             use uniqueness?                         {0,1}
        11:             use distribution?                       {0,1}
        12:             filter uniqueness?                      {0,1}
        13:             filter distribution?                    {0,1}
        14:             use superpixel color?                   {0,1}

    === extractor_tweak_vector: ===
    The following tables provide information about the usage of the tweak vectors for the feature extractor modules.

    CONTOUR
    -------
        INDEX           EXPLANATION                             ACCEPTED VALUES
        0:              number of fourier components            {n | n el N+, n>=2}

    HISTOGRAM
    ---------
        INDEX           EXPLANATION                             ACCEPTED VALUES
        0:              number of hue histogram bins            {n | n el N+, n>=1}
        1:              number of saturation histogram bins     {n | n el N+, n>=1}
        2:              number of value histogram bins          {n | n el N+, n>=1}
        3:              use channel-wise autocorrelation        {0,1}
        4:              use color histogram of whole image      {0,1}

    CONTOUR_HISTOGRAM
    -----------------
        INDEX           EXPLANATION                             ACCEPTED VALUES
        0:              number of fourier components            {n | n el N+, n>=2}
        1:              number of hue histogram bins            {n | n el N+, n>=1}
        2:              number of saturation histogram bins     {n | n el N+, n>=1}
        3:              number of value histogram bins          {n | n el N+, n>=1}
        4:              use channel-wise autocorrelation        {0,1}
        5:              use color histogram of whole image      {0,1}


    EXAMPLE: FeatureGenerator.cfg
    -----------------------------------------------------------------------------------------------
    log_file = ..\FeatureGenerator.log
    directories_file = ..\directories.txt
    include_subdirs = 0
    output_directory = ..\feature_generator_out
    delete_old_features = 0
    features_file = ..\fg_features.txt
    processed_images_file = ..\fg_processed_images.txt
    garbage_file = ..\fg_files_with_no_salient_regions.txt
    detector_type = saliency_filters
    detector_tweak_vector = 400 5 1 0.25 20.0 3 0.1 0.033333 0.033333 1 1 1 0 0 0
    blur_kernel_size = 9
    threshold = 0.4
    min_salient_region_size = 400
    use_grabcut = 1
    grabcut_foreground_probability = 0.2
    extractor_type = contour_histogram
    extractor_tweak_vector = 40 10 10 10 0
    save_saliency_maps = 1
    saliency_maps_file = ..\fg_saliency_maps.txt
    save_saliency_masks = 1
    saliency_masks_file = ..\fg_saliency_masks.txt
    symlink_garbage_files = 1
    -----------------------------------------------------------------------------------------------


    OUTPUT:
    - a log-file
    - a file that lists all correctly processed images
    - a list that lists all images that could not be processed due to technical problems
    - a file that lists the extracted feature vectors; that list corresponds to the list of processed images
    - (optional) saliency maps for each image
    - (optional) saliency masks for each image
    - (optional) symbolic links to files that caused errors
    - a file that stores the paths of all saliency maps
    - a file that stores the paths of all saliency masks


2.2 Clusterer #####################################################################################

    The Clusterer takes a config file that is to be filled with the listed keys and values

    INPUT:
    VARIABLE                        EXPLANATION                                                                         ACCEPTED VALUES
    --------------------------------------------------------------------------------------------------------------------------------------------------------------
    log_file                        path to the log file                                                                path to a file
    features_file                   a file that stores the feature vectors                                              path to a file
    images_file                     a file that stores the corresponding image-paths                                    path to a file
    clusterer_type                  the type of clusterer that is to be used                                            { flannkmeans, outlier, optics }
    clusterer_tweak_vector          a vector that parameterizes the clusterer                                           vector of real numbers delimited by spaces
    output_directory                path of the output directory                                                        path to a directory
    membership_probabilities_file   a file that stores the cluster membership probabilities for each image              path to a file
    membership_mappings_file        a file that assigns one cluster to each image                                       path to a file
    result_file_prefix              a prefix for folders that represent clusters into which images are linked           a string without spaces
    result_filenames_file           a file for the paths to all text files which keep the partitioned image paths       path to a file
    symlink_results                 whether or not to create clustered symbolic links of the images                     {0,1}
    symlink_saliency_maps           whether or not to create clustered symbolic links of the saliency maps              {0,1}
    saliency_maps_file              a file that points to all saliency maps                                             path to a file
    --------------------------------------------------------------------------------------------------------------------------------------------------------------

    === clusterer_tweak_vector: ===
    The following tables provide information about the usage of the tweak vectors for the clustering modules.

    FLANNKMEANS
    -----------
        INDEX           EXPLANATION                             ACCEPTED VALUES
        0:              number of clusters hint{n               N+

    OUTLIER
    -------
        INDEX           EXPLANATION                             ACCEPTED VALUES
        0:              number of outliers                      N+

    OPTICS
    ------
        INDEX           EXPLANATION                             ACCEPTED VALUES
        0:              find n clusters / find persistent ones  {0,1}
        1:              epsilon, if <=0: epsilon:=INFINITY      R
        2:              min_pts                                 N+
        3:              number of clusters / persistence value  {n | n el N+, n>=1}
        4:              outlier threshold, if <=0: :=INFINITY   R


        The optics module creates also two files in their working directory:
            - optics_ordered_feature_indices.txt
            - optics_reachability_distances.txt
        both files can be used to work with the OPTICSAnalyzer.


    EXAMPLE: Clusterer.cfg
    -----------------------------------------------------------------------------------------------
    log_file = ..\Clusterer.log
    features_file = ..\fg_features.txt
    images_file = ..\fg_processed_images.txt
    clusterer_type = optics
    clusterer_tweak_vector = 0 3 50 3 0
    output_directory = ..\clusterer_out
    membership_probabilities_file = ..\c_membership_probabilities.txt
    membership_mappings_file = ..\c_membership_mappings.txt
    result_file_prefix = c
    result_filenames_file = ..\c_class_files.txt
    symlink_results = 1
    symlink_saliency_maps = 0
    saliency_maps_file = ..\fg_saliency_maps.txt
    -----------------------------------------------------------------------------------------------


    OUTPUT:
    - a log file
    - a file that stores the cluster membership probabilities for each image
    - a file that stores the cluster assignments for each image
    - n result files; each file represents one cluster and stores the paths of the corresponding images
    - a file that stores the paths to the n cluster result files
    - (optional) folders that represent clusters with symbolic links of the corresponding images
    - (optional) symbolic links of the corresponding saliency maps in the same folders


2.3 Evaluation ####################################################################################

    The Evaluation application takes a config file that is to be filled with the listed keys and values

    INPUT:
    VARIABLE                        EXPLANATION                                                                 ACCEPTED VALUES
    --------------------------------------------------------------------------------------------------------------------------------------------------
    log_file                        path to the log file                                                        path to a file
    membership_probabilities_file   a file that stores the cluster membership probabilities for each image      path to a file
    membership_mappings_file        a file that assigns one cluster to each image                               path to a file
    cluster_file_paths_file         a file that stores the paths to all cluster files                           path to a file
    image_db_directory              root directory to the benchmark image database                              path to a directory
    mode                            the evaluation mode, either precision/recall or bayes inference             { precision_recall / bayes_inference }
    posteriors_file                 a file that stores the bayesian inference posteriors                        path to a file
    precision_recall_file           a file that stores the precision/recall values                              path to a file
    class_name_file                 a file that stores the inferred class names for each cluster                path to a file
    --------------------------------------------------------------------------------------------------------------------------------------------------

    EXAMPLE: Evaluation.cfg
    -----------------------------------------------------------------------------------------------
    log_file = ..\Evaluation.log
    membership_probabilities_file = ..\c_membership_probabilities.txt
    membership_mappings_file = ..\c_membership_mappings.txt
    cluster_file_paths_file = ..\c_class_files.txt
    image_db_directory = H:\256_ObjectCategories
    mode = precision_recall
    posteriors_file = ..\eval_posteriors.txt
    precision_recall_file = ..\eval_precision_recall_file.txt
    class_name_file = ..\eval_class_name_file.txt
    -----------------------------------------------------------------------------------------------


    OUTPUT:
    - a log file
    - (optional) a file that stores the posterior probabilities of each image to belong to the assigned cluster
    - (optional) a file that stores the precision/recall measures for each class/cluster
    - a file that stores the class assignments to each cluster


2.4 OPTICSAnalyzer ################################################################################

    OPTICSAnalyzer asks for a config file at startup. The config file must look somehow like this:

    Example: configuration.cfg
    -------------------------------------------------------------------------------------------------------------
    <?xml version="1.0" encoding="utf-8" ?>
    <appSettings>
        <add key="images_file" value=".\..\..\..\fg_processed_images.txt" />
        <add key="images_optics_ordered_file" value=".\..\..\..\Clusterer\optics_ordered_feature_indices.txt" />
        <add key="reachabilities_file" value=".\..\..\..\Clusterer\optics_reachability_distances.txt" />
    </appSettings>
    -------------------------------------------------------------------------------------------------------------

    The specified files must exist and hold valid entries.



3. EXTERNAL LIBRARIES #############################################################################
###################################################################################################

    LIBRARY                                                 LICENSE                                         WEBSITE

    - OpenCV                                                3-clause BSD License                            http://opencv.org
    - Boost                                                 Boost Software License                          http://boost.org
    - SaliencyFilters impl. by Philipp Krähenbühl           --- no licensing ---                            http://www.philkr.net
    - Persistence1D   by Yeara Kozlov and Tino Weinkauf     --- no licensing ---                            http://www.mpi-inf.mpg.de/~weinkauf/notes/persistence1d.html
    - rlutil          by Tapio Vierros                      Do What The Fuck You Want To Public License     http://tapiov.net/rlutil



4. HISTORY ########################################################################################
###################################################################################################

150717      - added link to the Caltech-256 image database
            - mentioned the doxygen documentation folder

150713      - added Philipp Krähenbühl's SaliencyFilters to the EXTERNAL LIBRARIES section

150625      -updated use histogram of whole image

150611      -creation of the readme file
            -made tweak-vectors foolproof



# END OF FILE #

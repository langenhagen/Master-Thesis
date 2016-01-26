"""Deletes several files and foldersd in a directory
"""

import os
import shutil


directories = [
                "clusterer_out",       
                "feature_generator_out",
                "c_class_files.txt",
                "c_cluster_means.txt",
                "c_membership_mappings.txt",
                "c_membership_probabilities.txt",
                "Clusterer.log",
                "Evaluation.log",
                "FeatureGenerator.log",
                "fg_features.txt",
                "fg_files_with_no_salient_regions.txt",
                "fg_processed_images.txt",
                "fg_saliency_maps.txt",
                "fg_saliency_masks.txt",
                "eval_posteriors.txt",
                "eval_class_name_file.txt",
                "eval_precision_recall_file.txt"
                ]


print( "About to delete files:\n", directories,"\n" ) 
if input("Proceed? (y): ") != "y":
    sys.exit(1)                
                
for d in directories:  
    if os.path.exists(d):
        if os.path.isfile( d):
            os.remove(d)
        else:
            shutil.rmtree(d)

print("DONE.")
# input()              
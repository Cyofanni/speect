######################################################################################
##                                                                                  ##
## AUTHOR  : Giovanni Mazzocchin                                                    ##
## DATE    : 05 July 2016                                                           ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## CMakeList for Rule Based Feature processor plug-in                               ##
##                                                                                  ##
##                                                                                  ##
######################################################################################

#------------------------------------------------------------------------------------#
#                             Define plug-in                                         #
#------------------------------------------------------------------------------------#

speect_plugin_definition(RuleBasedFeatures_UttProc "SRuleBasedFeaturesUttProc" 1 0 0)


#------------------------------------------------------------------------------------#
#                        Configure plugin_info.h.in                                  #
#------------------------------------------------------------------------------------#

set(description "Get features from rules.")

# Minimum required Speect Engine version
set(major_min 1)
set(minor_min 0)

speect_plugin_configure_info(${description} ${major_min} ${minor_min})


#------------------------------------------------------------------------------------#
#                              Include Labels Data Collector                         #
#------------------------------------------------------------------------------------#

#speect_include_plugin(HTS_Labels_Data_Collector_FeatProc 1 0)

#------------------------------------------------------------------------------------#
#                               Source files                                         #
#------------------------------------------------------------------------------------#

# Lists of all the source files
include(sources)


#------------------------------------------------------------------------------------#
#                        Plug-in shared object                                       #
#------------------------------------------------------------------------------------#

speect_plugin_create()


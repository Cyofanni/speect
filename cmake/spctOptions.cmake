######################################################################################
##                                                                                  ##
## AUTHOR  : Aby Louw                                                               ##
## DATE    : 7 June 2010                                                            ##
##                                                                                  ##
######################################################################################
##                                                                                  ##
## Speect build options                                                             ##
##                                                                                  ##
##                                                                                  ##
######################################################################################


#------------------------------------------------------------------------------------#
#                        Shared libraries (Default ON)                               #
#------------------------------------------------------------------------------------#

# Choose static or shared libraries.
#option(BUILD_SHARED_LIBS "Build Shared Libraries" on) 

set(SPCT_LIB_TYPE SHARED) 

# we support only shared libraries for now.
#if(BUILD_SHARED_LIBS)
  # User wants to build Static Libraries, so change the SPCT_LIB_TYPE variable to CMake keyword 'STATIC'
#  set(LIB_TYPE STATIC)
#endif(BUILD_SHARED_LIBS)

#------------------------------------------------------------------------------------#
#                              Library suffix                                        #
#------------------------------------------------------------------------------------#

#
# On some 64-bit platforms, libraries should be installed into `lib64'
# instead of `lib'.  Set this to 64 to do that.
#
set(LIB_SUFFIX "" CACHE STRING "Suffix for 'lib' directories, e.g. '64'")


#------------------------------------------------------------------------------------#
#                     Multi threaded support  (Default OFF)                          #
#------------------------------------------------------------------------------------#

# Choose to compile with thread support or not.
option(WANT_THREADS "Enable multi threaded support." off)


#------------------------------------------------------------------------------------#
#                                Developers options                                  #
#------------------------------------------------------------------------------------#

# Halt at warnings (Default OFF)
option(STRICT_WARN "Halt at warnings." off)

# Warn about declarations after statements (Default OFF)
option(WARN_DECL_AFTER_STMT "Warn about declarations after statements (GCC)." off)

# Abort on fatal errors (Default OFF)
option(ERROR_ABORT_FATAL "Abort on fatal errors." off)

# Do safe cast of objects, slower than non-safe cast (Default ON)
option(SAFE_CAST "Do safe cast of objects, slower than non-safe cast." on)

# Use Speect's error handling facilities (Default ON)
option(ERROR_HANDLING "Speect error handling facilities." on)


#------------------------------------------------------------------------------------#
#                              Tests (Default OFF)                                   #
#------------------------------------------------------------------------------------#

option(WANT_TESTS "Include the test suite in the compilation." off)


#------------------------------------------------------------------------------------#
#                              Examples (Default OFF)                                #
#------------------------------------------------------------------------------------#

option(WANT_EXAMPLES "Include the example suite in the compilation." off)


#------------------------------------------------------------------------------------#
#                            SWIG Interface wrappers                                 #
#------------------------------------------------------------------------------------#

# Python (Default ON)
#--------------------

# default off in speect/cmake/spctOS.cmake if on WIN32
option(WANT_PYTHON_WRAPPER "Enable generation of the Python wrapper." on)

option(WANT_PYTHON_3 "Generate wrapper code for Python versions >= 3" off)

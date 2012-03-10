
# additional include paths necessary to compile the C++ library
AM_CXXFLAGS = -I$(top_srcdir)/chaos/common @AM_CXXFLAGS@

###############################################################################
# THE LIBRARIES TO BUILD
###############################################################################

# the library names to build (note we are building static libs only)
lib_LTLIBRARIES = libchaos_common.la

# where to install the headers on the system
libchaos_common.la_adir = $(includedir)/chaos/common

# the list of header files that belong to the library (to be installed later)
libmylib_a_HEADERS =    \
                        ChaosCommon.h version.h cconstants.h\

# the sources to add to the library and to add to the source distribution
libmylib_a_SOURCES =    \
                        $(libmylib_a_HEADERS) \
                        myClass.cpp 
						
###############################################################################

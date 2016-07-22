###############################################################################
# CMake module to search for the mpv libraries.
#
# WARNING: This module is experimental work in progress.
#
# Based one FindVLC.cmake by:
# Copyright (c) 2011 Michael Jansen <info@michael-jansen.biz>
# Modified by Tobias Hieta <tobias@hieta.se>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
###############################################################################

#
### Global Configuration Section
#
SET(_CONNMAN-QT5_REQUIRED_VARS CONNMAN-QT5_INCLUDE_DIR CONNMAN-QT5_LIBRARY)

#
### Connman-qt5 uses pkgconfig.
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_ QUIET connman-qt5)
endif(PKG_CONFIG_FOUND)

#
### Look for the include files.
#
find_path(
    CONNMAN-QT5_INCLUDE_DIR
    NAMES networkmanager.h
    PATH_SUFFIXES connman-qt5
    DOC "connman-qt5 include directory"
    )
mark_as_advanced(CONMANN-QT5_INCLUDE_DIR)
set(CONNMAN-QT5_INCLUDE_DIRS ${CONNMAN-QT5_INCLUDE_DIR})

#
### Look for the libraries 
#
find_library(
    CONNMAN-QT5_LIBRARY
    NAMES connman-qt5
    PATH_SUFFIXES lib${LIB_SUFFIX}
    )

get_filename_component(_CONNMAN-QT5_LIBRARY_DIR ${CONNMAN-QT5_LIBRARY} PATH)
mark_as_advanced(CONNMAN-QT5_LIBRARY)

#
### Check if everything was found and if the version is sufficient.
#
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    CONNMAN-QT5
    REQUIRED_VARS ${_CONNMAN-QT5_REQUIRED_VARS}
    VERSION_VAR CONNMAN-QT5_VERSION_STRING
    )

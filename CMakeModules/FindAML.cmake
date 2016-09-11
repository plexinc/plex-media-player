###############################################################################
# Find AML
###############################################################################
#
#  AML_FOUND
#  AML_INCLUDE_DIR
#  AML_LIBRARY
#
###############################################################################

find_path(AML_INCLUDE_DIR amcodec/codec.h
	PATH_SUFFIXES include
	PATHS /usr
)

find_library(AML_LIBRARY
	amcodec
	PATH_SUFFIXES lib
	PATHS /usr
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AML DEFAULT_MSG AML_LIBRARY)

mark_as_advanced(AML_LIBRARY AML_INCLUDE_DIR)

set(AML_FOUND ${AML_FOUND})


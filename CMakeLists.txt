cmake_minimum_required(VERSION 3.13.0)

project(mwc
    VERSION 1.0
    DESCRIPTION "Count the number of lines, words, and characters in a file.")

option(MWC_BUILD_TESTS "Enable mwd tests" OFF)

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Installation directory for manual pages")

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
enable_testing()

#=================================================================================================#
#  mwc                                                                                            #
#=================================================================================================#

set(MWC_PRIVATE_HEADERS
	source/mwc_cmdl.h)

set(RNAF_SOURCES
	source/rnaf.c
	source/memory_utils.c
	source/string_utils.c)

add_executable(mwc
    source/mwc.c
    source/mwc_cmdl.c)

if(NOT SKIP_INSTALL_ALL)
    install(TARGETS mwc
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}")
endif()
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL )
    install(FILES ${RNAF_PUBLIC_HEADERS} DESTINATION "${INSTALL_INC_DIR}")
endif()

# Specify compiler options based on OS
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_options(mwc PRIVATE -Wall -O3)
	target_link_options(mwc PRIVATE -Wall -O3 -flto)

elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin") # Apple
	target_compile_options(mwc PRIVATE -Wall -Wextra -Wpedantic -O3)
	target_link_options(mwc PRIVATE -Wall -Wextra -Wpedantic -O3 -flto)

else()
    message( SEND_ERROR "System ${CMAKE_SYSTEM_NAME} currently not supported.")
endif()

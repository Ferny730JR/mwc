cmake_minimum_required(VERSION 3.13.0)

project(mwc
    LANGUAGES C
    VERSION 1.0
    DESCRIPTION "Count the number of lines, words, and characters in a file.")

enable_language(C)

option(MWC_BUILD_TESTS "Enable mwd tests" OFF)

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Installation directory for manual pages")

include(CheckIncludeFile)
include(CheckFunctionExists)
enable_testing()

# Check for required headers
CHECK_INCLUDE_FILE("stdio.h" HAVE_STDIO_H)
CHECK_INCLUDE_FILE("stdbool.h" HAVE_STDBOOL_H)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILE("ctype.h" HAVE_CTYPE_H)
CHECK_INCLUDE_FILE("pthread.h" HAVE_PTHREAD_H)
CHECK_INCLUDE_FILE("semaphore.h" HAVE_SEMAPHORE_H)
CHECK_INCLUDE_FILE("stdatomic.h" HAVE_STDATOMIC_H)

# Check for library dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Add your executable and link against required libraries
add_executable(mwc
    source/mwc.c
    source/mwc_cmdl.c
    source/parallel_helpers.c)

target_link_libraries(mwc PRIVATE Threads::Threads)

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

elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(mwc PRIVATE /W4 /O3)
    target_link_options(mwc PRIVATE /LTCG)

else()
    message( SEND_ERROR "System ${CMAKE_SYSTEM_NAME} currently not supported.")
endif()

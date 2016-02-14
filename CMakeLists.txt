##
## CMakeLists.txt for code that produces
## - a sharable library with dictionary
## - an executable
##

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)

project(calibmon)

##--- requires Root compiled and installed with cmake
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net Rint Graf Gpad Thread)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR})

ROOT_GENERATE_DICTIONARY(calibmonDict calibmon.h LINKDEF calibmonLinkDef.h)

# --- Create a shared library with generated dictionary
add_library(calibmon SHARED calibmon.cxx calibmonDict.cxx KeyPress.cxx KeyPressTimer.cxx)
# message(ROOT_LIBRARIES " " ${ROOT_LIBRARIES})
target_link_libraries(calibmon ${ROOT_LIBRARIES})

# --- Create  a main program using the library
add_executable(calibmonrun calibmonrun.cxx)
target_link_libraries(calibmonrun calibmon)

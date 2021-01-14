######### CMake Version #####################
cmake_minimum_required(VERSION 3.15)
#############################################


######### Info ##############################
MESSAGE( STATUS ">> CMAKE_C_COMPILER:        " ${CMAKE_C_COMPILER} )
MESSAGE( STATUS ">> CMAKE_CXX_COMPILER:      " ${CMAKE_CXX_COMPILER} )
#############################################


######### Project Name ######################
project( chaiscript_sandbox )
SET( EXECUTABLE_NAME run )
#############################################


######### Have the binary placed into the source head
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR} )
### Output paths for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
#############################################
set( CMAKE_DISABLE_SOURCE_CHANGES  ON )
set( CMAKE_DISABLE_IN_SOURCE_BUILD ON )
#############################################


#############################################
add_executable( ${EXECUTABLE_NAME} main.cpp )

target_include_directories( ${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/ChaiScript/include
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/ChaiScript_Extras/include
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/filesystem/include
    ${CMAKE_CURRENT_LIST_DIR}/thirdparty/fmt/include )

target_compile_definitions( ${EXECUTABLE_NAME} PRIVATE FMT_HEADER_ONLY )

set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY CXX_EXTENSIONS OFF)
#############################################
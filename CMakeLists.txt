cmake_minimum_required( VERSION 3.14 FATAL_ERROR )

project( safe_api C CXX )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

add_subdirectory( api )
add_subdirectory( task )

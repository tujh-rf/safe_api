set( api_impl1 cpp_with_classes_impl )

set( targets ${api_impl1} )

set( api1_sources
        "main.cpp"
)
source_group( "Simpliest C++ implementation" ${api1_sources} )

add_executable( ${api_impl1} ${api1_sources} )

add_dependencies( ${api_impl1} ${api_library} )

target_include_directories( ${api_impl1} PRIVATE ${api_root_dir} )
target_link_libraries( ${api_impl1} PRIVATE ${api_library} )

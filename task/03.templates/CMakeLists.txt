set( api_impl3 cpp_templates_impl )

set( targets ${api_impl3} )

set( api3_sources
        "main.cpp"
)
source_group( "C++ with templates" ${api3_sources} )

add_executable( ${api_impl3} ${api3_sources} )

add_dependencies( ${api_impl3} ${api_library} )

target_include_directories( ${api_impl3} PRIVATE ${api_root_dir} )
target_link_libraries( ${api_impl3} PRIVATE ${api_library} )

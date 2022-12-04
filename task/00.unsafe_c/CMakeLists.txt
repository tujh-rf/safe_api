set( api_impl0 pure_c_impl )

set( targets ${api_impl0} )

set( api0_sources
        "main.c"
)
source_group( "Original Implementation on pure C" ${api0_sources} )

add_executable( ${api_impl0} ${api0_sources} )

add_dependencies( ${api_impl0} ${api_library} )

target_include_directories( ${api_impl0} PRIVATE ${api_root_dir} )
target_link_libraries( ${api_impl0} PRIVATE ${api_library} )

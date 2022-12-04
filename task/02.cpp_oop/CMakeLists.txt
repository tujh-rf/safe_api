set( api_impl2 cpp_oop_impl )

set( targets ${api_impl2} )

set( api2_sources
        "main.cpp"
)
source_group( "C++ with OOP" ${api2_sources} )

add_executable( ${api_impl2} ${api2_sources} )

add_dependencies( ${api_impl2} ${api_library} )

target_include_directories( ${api_impl2} PRIVATE ${api_root_dir} )
target_link_libraries( ${api_impl2} PRIVATE ${api_library} )

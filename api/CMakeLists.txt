set( api_library api )

set( targets ${api_library} )

set( api_root_dir ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE )
set( api_library ${api_library} PARENT_SCOPE )

set( api_sources
        "api.h"
        "api_impl.c"
)
source_group( "API Library" ${api_sources} )

add_library( ${api_library} STATIC ${api_sources} )

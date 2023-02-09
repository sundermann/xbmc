if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_NDL_directmedia NDL_directmedia REQUIRED)
endif()

find_path(NDL_directmedia_INCLUDE_DIR NAMES NDL_directmedia.h
        PATHS ${PC_NDL_directmedia_INCLUDEDIR})
find_library(NDL_directmedia_LIBRARY NAMES NDL_directmedia libNDL_directmedia
        PATHS ${PC_NDL_directmedia_LIBDIR})

set(NDL_directmedia_VERSION ${PC_NDL_directmedia_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NDL_directmedia
        REQUIRED_VARS NDL_directmedia_LIBRARY NDL_directmedia_INCLUDE_DIR
        VERSION_VAR NDL_directmedia_VERSION)

if(NDL_directmedia_FOUND)
    set(NDL_directmedia_LIBRARIES ${NDL_directmedia_LIBRARY})
    set(NDL_directmedia_INCLUDE_DIRS ${NDL_directmedia_INCLUDE_DIR})

    if(NOT TARGET NDL_directmedia::NDL_directmedia)
        add_library(NDL_directmedia::NDL_directmedia UNKNOWN IMPORTED)
        set_target_properties(NDL_directmedia::NDL_directmedia PROPERTIES
                IMPORTED_LOCATION "${NDL_directmedia_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${NDL_directmedia_INCLUDE_DIR}")
    endif()
endif()

mark_as_advanced(NDL_directmedia_INCLUDE_DIR NDL_directmedia_LIBRARY)
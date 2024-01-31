cmake_minimum_required(VERSION 3.14)

include(GNUInstallDirs)

set(ADDITIONAL_MODULES_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${ADDITIONAL_MODULES_DIR}")

project(nameof
    VERSION "0.10.4"
    HOMEPAGE_URL "https://github.com/Neargye/nameof"
    DESCRIPTION "A library that provides nameof macros and functions to simply obtain the name of a variable, type, function, macro, and enum."
    LANGUAGES CXX
)
set(CPACK_PACKAGE_VENDOR "Daniil Goncharov")

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(NAMEOF_OPT_BUILD_EXAMPLES "Build nameof examples" ${IS_TOPLEVEL_PROJECT})
option(NAMEOF_OPT_BUILD_TESTS "Build and perform nameof tests" ${IS_TOPLEVEL_PROJECT})
option(NAMEOF_OPT_INSTALL "Generate and install nameof target" ${IS_TOPLEVEL_PROJECT})

if(NAMEOF_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(NAMEOF_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

include(CMakePackageConfigHelpers)

set(EXPORT_NAMESPACE "${PROJECT_NAME}::")

add_library("${PROJECT_NAME}" INTERFACE)
add_library("${EXPORT_NAMESPACE}${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")
set(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(${PROJECT_NAME}
        INTERFACE
            $<BUILD_INTERFACE:${INCLUDES}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(NAMEOF_OPT_INSTALL)
    list(APPEND CMAKE_MODULE_PATH "${ADDITIONAL_MODULES_DIR}/GenPkgConfig")
    include(GenPkgConfig)
    include(CPackComponent)
    include(CMakePackageConfigHelpers)
    
    string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
    install(TARGETS "${PROJECT_NAME}"
        EXPORT ${PROJECT_NAME}
        INCLUDES
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
            # COMPONENT "${SDK_COMPONENT_NAME}" # component is not allowed for includes! Headers are installed separately! Includes only marks the headers for export
    )

    file(GLOB_RECURSE HEADERS "${INCLUDES}/*.h" "${INCLUDES}/*.hxx" "${INCLUDES}/*.hpp")
    foreach(headerFile ${HEADERS})
        get_filename_component(headerFileParentDir "${headerFile}" DIRECTORY)
        file(RELATIVE_PATH headerFileRelParentDir "${INCLUDES}" "${headerFileParentDir}")

        install(FILES "${headerFile}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${headerFileRelParentDir}"
        )
    endforeach()

    set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
    set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_PACKAGE_NAME "lib${CPACK_PACKAGE_NAME}-dev")
    set(CPACK_RPM_PACKAGE_NAME "lib${CPACK_PACKAGE_NAME}-devel")
    set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
    set(CPACK_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_MAINTAINER}")
    set(CPACK_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_RPM_COMPONENT_INSTALL ON)
    set(CPACK_NSIS_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_COMPRESSION_TYPE "xz")

    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

    set(CMAKE_CONFIG_FILE_BASENAME "${PROJECT_NAME}Config.cmake")
    set(CMAKE_EXPORT_FILE_BASENAME "${PROJECT_NAME}Export.cmake")
    set(CMAKE_CONFIG_VERSION_FILE_BASENAME "${PROJECT_NAME}ConfigVersion.cmake")
    set(CMAKE_CONFIG_VERSION_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CONFIG_VERSION_FILE_BASENAME}")

    export(TARGETS "${PROJECT_NAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        FILE "${CMAKE_EXPORT_FILE_BASENAME}"
        EXPORT_LINK_INTERFACE_LIBRARIES
    )

    install(EXPORT "${PROJECT_NAME}"
        FILE "${CMAKE_CONFIG_FILE_BASENAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}"
    )

    write_basic_package_version_file(
        "${CMAKE_CONFIG_VERSION_FILE_NAME}"
        #VERSION "100500.100500.100500"  # any version of same bitness suits. CMake cannot compare to infinity, so use a large number we expect to be greater than any future version
        VERSION ${_VERSION}
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT
    )
    install(FILES "${CMAKE_CONFIG_VERSION_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}"
    )

    configure_pkg_config_file("${PROJECT_NAME}"
        NAME "${PROJECT_NAME}"
        VERSION "${PROJECT_VERSION}"
        DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}"
        URL "${CPACK_PACKAGE_HOMEPAGE_URL}"
        INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR_ARCHIND}"
        INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    include(CPack)
endif()

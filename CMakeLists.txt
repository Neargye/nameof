cmake_minimum_required(VERSION 3.22)

project(nameof
    VERSION "0.10.6"
    HOMEPAGE_URL "https://github.com/Neargye/nameof"
    DESCRIPTION "A library that provides nameof macros and functions to simply obtain the name of a variable, type, function, macro, and enum."
    LANGUAGES CXX
)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(NAMEOF_OPT_BUILD_EXAMPLES "Build nameof examples" ${PROJECT_IS_TOP_LEVEL})
option(NAMEOF_OPT_BUILD_TESTS "Build and perform nameof tests" ${PROJECT_IS_TOP_LEVEL})
option(NAMEOF_OPT_INSTALL "Generate and install nameof target" ${PROJECT_IS_TOP_LEVEL})

set(EXPORT_NAMESPACE "${PROJECT_NAME}::")

add_library("${PROJECT_NAME}" INTERFACE)
add_library("${EXPORT_NAMESPACE}${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")
set(INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${INCLUDES}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

if(NAMEOF_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(NAMEOF_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if(NAMEOF_OPT_INSTALL)
    include("${CMAKE_CURRENT_LIST_DIR}/cmake/GenPkgConfig/GenPkgConfig.cmake")

    set(CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
    if(CMAKE_LIBRARY_ARCHITECTURE)
        string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" CMAKE_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR_ARCHIND}")
    endif()

    install(TARGETS "${PROJECT_NAME}"
        EXPORT ${PROJECT_NAME}
    )
    install(FILES "${INCLUDES}/nameof.hpp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}"
    )

    set(CMAKE_CONFIG_FILE_BASENAME "${PROJECT_NAME}Config.cmake")
    set(CMAKE_CONFIG_VERSION_FILE_BASENAME "${PROJECT_NAME}ConfigVersion.cmake")
    set(CMAKE_CONFIG_VERSION_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CONFIG_VERSION_FILE_BASENAME}")

    install(EXPORT "${PROJECT_NAME}"
        FILE "${CMAKE_CONFIG_FILE_BASENAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}"
    )

    write_basic_package_version_file(
        "${CMAKE_CONFIG_VERSION_FILE_NAME}"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT
    )
    install(FILES "${CMAKE_CONFIG_VERSION_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}"
    )

    configure_pkg_config_file(
        "${PROJECT_NAME}"
        NAME "${PROJECT_NAME}"
        VERSION "${PROJECT_VERSION}"
        DESCRIPTION "${PROJECT_DESCRIPTION}"
        URL "${PROJECT_HOMEPAGE_URL}"
        INSTALL_LIB_DIR "${CMAKE_INSTALL_LIBDIR_ARCHIND}"
        INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif()

if(NAMEOF_OPT_INSTALL AND PROJECT_IS_TOP_LEVEL)
    set(CPACK_PACKAGE_VENDOR "Daniil Goncharov")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
    set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_PACKAGE_NAME "libnameof-dev")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "noarch")
    set(CPACK_RPM_PACKAGE_NAME "libnameof-devel")
    include(CPack)
endif()

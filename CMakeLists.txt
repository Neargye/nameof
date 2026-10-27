cmake_minimum_required(VERSION 3.22)

project(nameof
    VERSION "0.11.0"
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
    set(NAMEOF_INSTALL_LIBDIR_ARCHIND "${CMAKE_INSTALL_LIBDIR}")
    if(CMAKE_LIBRARY_ARCHITECTURE)
        string(REPLACE "/${CMAKE_LIBRARY_ARCHITECTURE}" "" NAMEOF_INSTALL_LIBDIR_ARCHIND "${NAMEOF_INSTALL_LIBDIR_ARCHIND}")
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

    set(NAMEOF_CONFIG_FILE_BASENAME "${PROJECT_NAME}Config.cmake")
    set(NAMEOF_CONFIG_VERSION_FILE_BASENAME "${PROJECT_NAME}ConfigVersion.cmake")
    set(NAMEOF_CONFIG_VERSION_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${NAMEOF_CONFIG_VERSION_FILE_BASENAME}")

    install(EXPORT "${PROJECT_NAME}"
        FILE "${NAMEOF_CONFIG_FILE_BASENAME}"
        NAMESPACE "${EXPORT_NAMESPACE}"
        DESTINATION "${NAMEOF_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}"
    )

    write_basic_package_version_file(
        "${NAMEOF_CONFIG_VERSION_FILE_NAME}"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY AnyNewerVersion
        ARCH_INDEPENDENT
    )
    install(FILES "${NAMEOF_CONFIG_VERSION_FILE_NAME}"
        DESTINATION "${NAMEOF_INSTALL_LIBDIR_ARCHIND}/cmake/${PROJECT_NAME}"
    )

    if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}" OR IS_ABSOLUTE "${NAMEOF_INSTALL_LIBDIR_ARCHIND}")
        # Absolute install directories cannot be expressed relative to the .pc file.
        set(NAMEOF_PC_PREFIX "${CMAKE_INSTALL_PREFIX}")
        set(NAMEOF_PC_INCLUDEDIR "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
    else()
        file(RELATIVE_PATH NAMEOF_PC_PREFIX
            "${CMAKE_CURRENT_BINARY_DIR}/pkgconfig-root/${NAMEOF_INSTALL_LIBDIR_ARCHIND}/pkgconfig"
            "${CMAKE_CURRENT_BINARY_DIR}/pkgconfig-root"
        )
        string(REGEX REPLACE "/$" "" NAMEOF_PC_PREFIX "${NAMEOF_PC_PREFIX}")
        set(NAMEOF_PC_PREFIX "\${pcfiledir}/${NAMEOF_PC_PREFIX}")
        set(NAMEOF_PC_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
    endif()
    # pkg-config splits Cflags on unescaped spaces.
    string(REPLACE " " "\\ " NAMEOF_PC_PREFIX "${NAMEOF_PC_PREFIX}")
    string(REPLACE " " "\\ " NAMEOF_PC_INCLUDEDIR "${NAMEOF_PC_INCLUDEDIR}")
    configure_file(cmake/nameof.pc.in nameof.pc @ONLY)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/nameof.pc"
        DESTINATION "${NAMEOF_INSTALL_LIBDIR_ARCHIND}/pkgconfig"
    )
endif()

if(NAMEOF_OPT_INSTALL AND PROJECT_IS_TOP_LEVEL)
    set(CPACK_PACKAGE_VENDOR "Daniil Goncharov")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Header-only C++17 library for obtaining expression, type and enum names")
    set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")
    set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    # Distribution file names: libnameof-dev_<version>_all.deb and libnameof-devel-<version>-1.noarch.rpm.
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
    set(CPACK_DEBIAN_PACKAGE_NAME "libnameof-dev")
    set(CPACK_DEBIAN_PACKAGE_SECTION "libdevel")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Daniil Goncharov <neargye@gmail.com>")
    set(CPACK_RPM_FILE_NAME RPM-DEFAULT)
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "noarch")
    set(CPACK_RPM_PACKAGE_NAME "libnameof-devel")
    include(CPack)
endif()

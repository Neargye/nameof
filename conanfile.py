#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, tools, CMake
from conans.model.version import Version
from conans.errors import ConanInvalidConfiguration

class NameofConan(ConanFile):
    name = 'nameof'
    version = '0.9.0'
    description = 'Header-only C++17 library provides nameof macros and functions to simply obtain the name of a variable, type, function, macro, and enum.'
    topics = (
        'conan',
        'nameof',
        'cplusplus',
        'enum-to-string',
        'serialization',
        'reflection',
        'header-only',
        'introspection',
        'compile-time'
    )
    url = 'https://github.com/Neargye/nameof'
    author = 'Daniil Goncharov <neargye@gmail.com>'
    license = 'MIT'
    generators = 'cmake_find_package'
    exports_sources = ['example/*', 'include/*', 'test/*', 'CMakeLists.txt', 'LICENSE']
    exports = ['LICENSE.md']
    _build_subfolder = 'build_subfolder'
    build_requires = []
    settings = ('os', 'compiler', 'build_type', 'arch')
    options = {
        'build_tests': [True, False],
        'build_examples': [True, False],
    }
    default_options = {
        'build_tests': False,
        'build_examples': False,
    }

    @property
    def supports_string_view(self):
        compiler = str(self.settings.compiler)
        version = Version(str(self.settings.compiler.version))
        if compiler == 'Visual Studio' and version >= Version('15'):
            return True
        if compiler == 'gcc' and version >= Version('7'):
            return True
        if compiler == 'icc' and version >= Version('19'):
            return True
        if compiler == 'clang' and version >= Version('5'):
            return True
        if compiler == 'apple-clang' and version >= Version('9'):
            return True
        return False

    def requirements(self):
        if self.options.build_tests:
            self.build_requires.append('Catch2/2.9.1@catchorg/stable')

    def configure(self):
        if not self.supports_string_view:
            raise ConanInvalidConfiguration('The specified compiler must support C++17')

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions['NAMEOF_OPT_BUILD_TESTS'] = self.options.build_tests
        cmake.definitions['NAMEOF_OPT_BUILD_EXAMPLES'] = self.options.build_examples
        cmake.configure(build_folder=self._build_subfolder)
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()
        if self.options.build_tests and not tools.cross_building(self.settings):
            cmake.test()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

    def package_id(self):
        self.info.header_only()

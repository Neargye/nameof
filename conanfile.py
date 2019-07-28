#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, tools, CMake
import os


class NameofConan(ConanFile):
    name = 'nameof'
    version = '0.9.0'
    description = 'Header-only C++17 library provides nameof macros and functions to obtain simple name of variable, type, function, macro, and enum.'
    topics = (
        'conan',
        'nameof',
        'introspection',
        'compile-time'
    )
    url = 'https://github.com/Neargye/nameof'
    author = 'Daniil Goncharov <neargye@gmail.com>'
    license = 'MIT'
    generators = 'cmake_find_package'
    exports_sources = ['example/*','include/*','test/*','CMakeLists.txt','LICENSE']
    exports = ['LICENSE.md']
    _build_subfolder = 'build_subfolder'
    settings = ('os', 'compiler', 'build_type', 'arch')
    options = {
        'build_tests': [True, False],
        'build_examples': [True, False],
    }
    default_options = {
        'build_tests': False,
        'build_examples': False,
    }

    def requirements(self):
        if self.options.build_tests:
            self.requires('Catch2/2.9.1@catchorg/stable')

    def config_options(self):
        needs_build = self.options.build_tests or self.options.build_examples
        if not needs_build:
            # remove all build settings since it is a header-only library
            self.options.remove(['os', 'compiler', 'build_type', 'arch'])

    def configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions['NAMEOF_OPT_BUILD_TESTS'] = self.options.build_tests
        cmake.definitions['NAMEOF_OPT_BUILD_EXAMPLES'] = self.options.build_examples
        cmake.configure(build_folder=self._build_subfolder)
        return cmake

    def build(self):
        cmake = self.configure_cmake()
        cmake.build()
        if self.options.build_tests:
            cmake.test()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()

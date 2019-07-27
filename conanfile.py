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
    build_requires = 'Catch2/2.9.1@catchorg/stable'

    def configure_cmake(self, build_tests_and_examples):
        cmake = CMake(self)
        if self.develop:
            self.output.info("Develop mode on, building with tests and examples")
        cmake.definitions['NAMEOF_OPT_BUILD_TESTS'] = build_tests_and_examples
        cmake.definitions['NAMEOF_OPT_BUILD_EXAMPLES'] = build_tests_and_examples
        cmake.configure(build_folder=self._build_subfolder)
        return cmake

    def build(self):
        cmake = self.configure_cmake(build_tests_and_examples = True)
        cmake.build()
        cmake.test()

    def package(self):
        cmake = self.configure_cmake(build_tests_and_examples = False)
        cmake.install()

# -*- coding: utf-8 -*-

from conans import ConanFile

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
        'compile-time'
    )
    url = 'https://github.com/Neargye/nameof'
    homepage = 'https://github.com/Neargye/nameof'
    author = 'Daniil Goncharov <neargye@gmail.com>'
    license = 'MIT'
    exports_sources = ["include/*", "LICENCE"]
    exports = ["LICENSE"]
    no_copy_source = True

    def package(self):
        self.copy("include/*")
        self.copy("LICENSE", dst="licenses")

    def package_id(self):
        self.info.header_only()

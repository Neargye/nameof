# -*- coding: utf-8 -*-

from conans import ConanFile
from conans.errors import ConanInvalidConfiguration

class NameofConan(ConanFile):
    name = "nameof"
    version = "0.9.0"
    description = "Header-only C++17 library provides nameof macros and functions to simply obtain the name of a variable, type, function, macro, and enum."
    topics = (
        "conan",
        "nameof",
        "cplusplus",
        "enum-to-string",
        "serialization",
        "reflection",
        "header-only",
        "compile-time"
    )
    url = "https://github.com/Neargye/nameof"
    homepage = "https://github.com/Neargye/nameof"
    author = "Daniil Goncharov <neargye@gmail.com>"
    license = "MIT"
    exports_sources = ["include/*", "LICENCE"]
    exports = ["LICENSE"]
    settings = "compiler"
    no_copy_source = True

    @property
    def supported_compiler(self):
        compiler = str(self.settings.compiler)
        version = str(self.settings.compiler.version)
        if compiler == "Visual Studio" and version >= "15":
            return True
        if compiler == "gcc" and version >= "7":
            return True
        if compiler == "clang" and version >= "5":
            return True
        if compiler == "apple-clang" and version >= "9":
            return True
        return False

    def configure(self):
        if not self.supported_compiler:
            raise ConanInvalidConfiguration("nameof: Unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).")

    def package(self):
        self.copy("include/*")
        self.copy("LICENSE", dst="licenses")

    def package_id(self):
        self.info.header_only()

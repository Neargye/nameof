#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    options = { 'nameof:build_tests': True, 'nameof:build_examples': True }
    always_true = lambda build: True

    builder = ConanMultiPackager()
    builder.add_common_builds()
    builder.update_build_if(always_true, new_options = options)
    builder.run()

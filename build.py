#!/usr/bin/env python
# -*- coding: utf-8 -*-

from bincrafters import build_template_header_only

if __name__ == "__main__":
    builder = build_template_header_only.get_builder()
    additional_options = { 'nameof:build_tests': True, 'nameof:build_examples': True }
    builder.update_build_if(lambda build: True, new_options=additional_options)
    builder.run()

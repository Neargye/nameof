#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cpt.packager import ConanMultiPackager

if __name__ == "__main__":
    builder = ConanMultiPackager()
    builder.add_common_builds()
    updated_builds = []
    for settings, options, env_vars, build_requires, reference in builder.items:
        options['prometheus-cpp:mode'] = 'pull'
        updated_builds.append([settings, options, env_vars, build_requires])
        options = options.copy()
        options['prometheus-cpp:mode'] = 'push'
        updated_builds.append([settings, options, env_vars, build_requires])
    builder.builds = updated_builds
    builder.run()

#!/usr/bin/env python
# --------------------------------------------------------------------
# build.py
#
# Author: Lain Musgrove (lain.proliant@gmail.com)
# Date: Thursday December 10, 2020
#
# Distributed under terms of the MIT license.
# -------------------------------------------------------------------

from pathlib import Path

from xeno.build import Recipe, build, default, provide, sh, target, factory

INCLUDES = [
    "-I./include",
    "-I./moonlight/include",
    "-I./jotdown/include",
    "-I./moonlight/deps/date/include"
]

ENV = dict(
    CC="clang++",
    CFLAGS=(
        "-g",
        *INCLUDES,
        "--std=c++2a",
        "-DMOONLIGHT_DEBUG",
        "-DMOONLIGHT_ENABLE_STACKTRACE",
        "-DMOONLIGHT_STACKTRACE_IN_DESCRIPTION",
    ),
    LDFLAGS=("-rdynamic", "-g", "-ldl", "-lpthread"),
)

# -------------------------------------------------------------------
@provide
def submodules():
    return sh("git submodule update --init --recursive")

# -------------------------------------------------------------------
@factory
def compile_test(src, headers):
    return sh(
        "{CC} {CFLAGS} {src} {LDFLAGS} -o {output}",
        env=ENV,
        src=src,
        output=Path(src).with_suffix(""),
        requires=headers,
    )

# -------------------------------------------------------------------
@provide
def test_sources():
    return Path.cwd().glob("test/*.cpp")

# -------------------------------------------------------------------
@provide
def lab_sources():
    return Path.cwd().glob("lab/*.cpp")

# -------------------------------------------------------------------
@provide
def headers():
    return Path.cwd().glob("include/timefilter/*.h")

# -------------------------------------------------------------------
@target
async def tests(test_sources, headers, submodules):
    return Recipe([compile_test(src, headers) for src in test_sources], setup=submodules)

# -------------------------------------------------------------------
@target
async def labs(lab_sources, headers, submodules):
    await submodules.resolve()
    return [compile_test(src, headers) for src in lab_sources]

# -------------------------------------------------------------------
@target
def run_tests(tests):
    return tuple(sh("{input}", input=test, cwd="test") for test in tests)

# -------------------------------------------------------------------
@default
def all(run_tests, labs):
    return [run_tests, labs]

# -------------------------------------------------------------------
if __name__ == "__main__":
    build()

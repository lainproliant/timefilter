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

from xeno.build import build, provide, task
from xeno.recipes import checkout, sh
from xeno.recipes.cxx import ENV, compile

# -------------------------------------------------------------------
DEPS = [
    "https://github.com/lainproliant/moonlight",
]

INCLUDES = [
    "-I./include",
    "-I./deps/moonlight/include",
    "-I./deps/moonlight/deps",
    "-I./deps/moonlight/deps/date/include",
]

ENV.update(
    append="CFLAGS,LDFLAGS",
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


# --------------------------------------------------------------------
@task(keep=True)
def deps():
    """Fetch third-party repos."""
    return [checkout(repo) for repo in DEPS]


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
@task(dep="deps")
def tests(test_sources, headers, deps):
    return [compile(src, headers=headers) for src in test_sources]


# -------------------------------------------------------------------
@task(dep="deps")
def labs(lab_sources, headers, deps):
    return [compile(src, headers=headers) for src in lab_sources]


# -------------------------------------------------------------------
@task
def run_tests(tests):
    return tuple(sh(f"{test.target}", test=test, cwd="test") for test in tests)


# -------------------------------------------------------------------
@task(default=True)
def all(run_tests, labs):
    return [run_tests, labs]


# -------------------------------------------------------------------
@task
def cc_json():
    """Generate compile_commands.json for IDEs."""
    return sh("intercept-build ./build.py compile:\\* -R; ./build.py -c compile:\\*")


# -------------------------------------------------------------------
if __name__ == "__main__":
    build()

#!/usr/bin/env python
# --------------------------------------------------------------------
# build.py
#
# Author: Lain Musgrove (lain.proliant@gmail.com)
# Date: Thursday December 10, 2020
#
# Distributed under terms of the MIT license.
# -------------------------------------------------------------------

import ast
import shlex
from pathlib import Path

from xeno.build import build, provide, recipe, task
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
    CFLAGS=(
        "-Wall",
        "-fpermissive",  # needed for g++ to respect "always_false<T>"
        *INCLUDES,
        "-DMOONLIGHT_DATETIME_DEBUG",
        "-DMOONLIGHT_ENABLE_STACKTRACE",
        "-DMOONLIGHT_STACKTRACE_IN_DESCRIPTION",
        "-x",
        "c++",
        "--std=c++2a",
    ),
    LDFLAGS=("-g", "-ldl", "-lpthread"),
)


# --------------------------------------------------------------------
@task(keep=True)
def dev_deps():
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
    return Path.cwd().glob("include/timefilter/**/*.h")


# -------------------------------------------------------------------
@task(dep="dev_deps")
def tests(test_sources, headers, dev_deps):
    return [compile(src, headers=headers) for src in test_sources]


# -------------------------------------------------------------------
@task(dep="dev_deps")
def labs(lab_sources, headers, dev_deps):
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
@task
def ycm_extra_conf():
    """Generates .ycm_extra_conf.py for the YouCompleteMe Vim plugin."""

    YCM_FILE_TEMPLATE = """
flags = [
FLAGS
]

def FlagsForFile(filename):
    return {
        'flags': flags,
        'do_cache': True
    }
""".strip()

    @recipe
    def mk_conf(target):
        with open(target, "w") as outfile:
            outfile.write(
                YCM_FILE_TEMPLATE.replace(
                    "FLAGS",
                    "\n".join(f"    '{flag}'" for flag in shlex.split(ENV["CFLAGS"])),
                )
            )

        return target

    return mk_conf(".ycm_extra_conf.py")


# -------------------------------------------------------------------
if __name__ == "__main__":
    build()

import os

# command line var=value variables
ARCH_CFLAGS = ARGUMENTS.get("ARCH", "")
PREFIX = ARGUMENTS.get("PREFIX", "/usr")
bin_DIR = os.path.join(PREFIX, "bin")
lib_DIR = os.path.join(PREFIX, "lib")

env = Environment()

# profile support
if ARGUMENTS.get("profile", False):
    env.MergeFlags(CPPFLAGS=["-pg"])

# debug support
if not ARGUMENTS.get("debug", False):
    env.MergeFlags("-O3")
else:
    env.MergeFlags("-O0 -g3 -ggdb")

# link time optimizations
env.MergeFlags("-ffunction-sections -fdata-sections")

# system dependent variables
env.MergeFlags(ARCH_CFLAGS)

# general compiler definitions
env.Append(CPPDEFINES=["-D__STDC_CONSTANT_MACROS"])

# exports
env.Export("env", "bin_DIR", 'lib_DIR')

# subdirs
env.SConscript("libjp4/SConscript")
env.SConscript("tools/SConscript")
env.SConscript("tests/SConscript")

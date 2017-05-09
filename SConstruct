#!python
import os

# To compile the module you need to run
# `scons p=linux` to compile for linux
# `scons p=windows` to compile for windows

# on linux you can optionally use `use_llvm=yes` to use clang instead of gcc

# Mac/Android will come soon as well. (help is appreciated :D)

# Set it with your own path
cpp_bindings = "../godot_cpp_bindings/include"
godot_headers = "../godot_headers"

env = Environment()

if ARGUMENTS.get("use_llvm", "no") == "yes":
    env["CXX"] = "clang++"

platform = ARGUMENTS.get("p", "linux")
bits = ARGUMENTS.get("bits", "64")

env.Append(CPPPATH=[godot_headers, cpp_bindings, cpp_bindings + '/godot_cpp', 'src'])
env.Append(LIBS=['godot_cpp_core', 'godot_cpp_bindings'])
env.Append(LIBPATH=[cpp_bindings, "lib"])

env.Append(CCFLAGS = ['-g','-O3'])
env.Append(CXXFLAGS = ['-std=c++14', '-Wwrite-strings'])

if platform == "linux":
    env.Append(LINKFLAGS = ['-Wl,-R,\'$$ORIGIN\''])

if platform == "windows":
    env.Append(LIBS=['godot.windows.opt.tools.' + bits])

def add_source(sources, name):
    sources.append("src/" + name)

def add_sources(sources, dir, extension):
    for f in os.listdir("src/" + dir):
        if f.endswith("." + extension):
            sources.append("src/" + dir + "/" + f)

# # sources has all the paths to files that should be compiled
# # I usually have one init.cpp at the top of the source tree.
# sources = ["init.cpp"]

sources = []

add_source(sources, "init.cpp")
add_source(sources, "sqlite.cpp")

add_sources(sources, "sqlite", "c")

library = env.SharedLibrary(target=('lib/sqlite'), source=sources)
Default(library)

#!python
import os

# GDNative headers path
godot_headers = "../godot_headers"

env = Environment(ENV = os.environ);

# Platform & bits
platform = ARGUMENTS.get("p", "windows");

# Include dir
env.Append(CPPPATH=[godot_headers, 'src']);

# Libraries
env.Append(LIBPATH=["lib"]);
env.Append(LIBS=['gdnative_wrapper_code']);

# Build flags
env.Append(CCFLAGS = ['-Zi']);

# Source lists
sources = [
	"src/gdsqlite.c",
	"src/sqlite/sqlite3.c"
];

library = env.SharedLibrary(target=("lib/gdsqlite"), source=sources);
Install("godot_project/lib/gdsqlite", source=library);

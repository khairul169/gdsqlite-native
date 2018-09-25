#!python
import os

# Environment
env = Environment(ENV = os.environ);

# Platform & bits
platform = ARGUMENTS.get('p', ARGUMENTS.get('platform', "windows"));
bits = ARGUMENTS.get('b', ARGUMENTS.get('bits', "64"));
target = ARGUMENTS.get('t', ARGUMENTS.get('target', "release"));

# Include dir
env.Append(CPPPATH=['godot_headers', 'src']);

if platform == 'linux':
	if ARGUMENTS.get('use_llvm', 'no') == 'yes':
		env['CXX'] = 'clang++';
	
	if bits == '32':
		env.Append(CCFLAGS = [ '-m32' ]);
		env.Append(LINKFLAGS = [ '-m32' ]);
	
	if bits == '64':
		env.Append(CCFLAGS = [ '-m64' ]);
		env.Append(LINKFLAGS = [ '-m64' ]);

if platform == 'windows':
	if target == 'debug':
		env['CCPDBFLAGS'] = '/Zi /Fd${TARGET}.pdb'
		env['PDB']='${TARGET.base}.pdb'
		env.Append(CCFLAGS = ['-EHsc', '/DEBUG', '-D_DEBUG', '/MDd'])

if platform == 'osx':
	env.Append(CCFLAGS = ['-g','-O3', '-arch', 'x86_64'])
	env.Append(LINKFLAGS = ['-arch', 'x86_64'])

# Source lists
sources = [
	"src/gdsqlite.c",
	"src/sqlite/sqlite3.c"
];

library = env.SharedLibrary(target=("lib/gdsqlite." + bits), source=sources);
Install("godot_project/lib/gdsqlite", source=library);

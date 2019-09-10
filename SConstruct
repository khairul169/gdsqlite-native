#!python
import os
import sys

# Environment
env = Environment(ENV = os.environ);

# Platform & bits
platform = ARGUMENTS.get('p', ARGUMENTS.get('platform', 'windows'));
bits = ARGUMENTS.get('b', ARGUMENTS.get('bits', '64'));
target = ARGUMENTS.get('t', ARGUMENTS.get('target', 'release'));
output = 'gdsqlite';
godotcpp_lib = 'libgodot-cpp';

if platform == 'linux':
	if ARGUMENTS.get('use_llvm', 'no') == 'yes':
		env['CXX'] = 'clang++';
	else:
		env['CXX']='g++';
	
	if target == 'debug':
		env.Append(CCFLAGS = ['-Og']);
	else:
		env.Append(CCFLAGS = ['-O3']);
	
	if bits == '64':
		env.Append(CCFLAGS = [ '-m64' ]);
		env.Append(LINKFLAGS = [ '-m64' ]);
	else:
		env.Append(CCFLAGS = [ '-m32' ]);
		env.Append(LINKFLAGS = [ '-m32' ]);

if platform == 'windows':
	if bits == '64':
		env = Environment(ENV = os.environ, TARGET_ARCH='amd64');
	else:
		env = Environment(ENV = os.environ, TARGET_ARCH='x86');
	
	if target == 'debug':
		env['CCPDBFLAGS'] = '/Zi /Fd${TARGET}.pdb'
		env['PDB']='${TARGET.base}.pdb'
		env.Append(CCFLAGS = ['-D_WIN32', '-EHsc', '/DEBUG', '-D_DEBUG', '/MDd'])
	else:
		env.Append(CCFLAGS = ['-D_WIN32', '/EHsc', '/O2', '/MD' ])

# If platform is OSX
if platform == 'osx':
	# If using Clang
	if ARGUMENTS.get('use_llvm', 'no') == 'yes':
		env['CXX'] = 'clang++'
	# Set compiler variables
	env.Append(CCFLAGS = [ '-g','-O3', '-arch', 'x86_64' ])
	env.Append(CXXFLAGS = [ '-std=c++14' ])
	env.Append(LINKFLAGS = [ '-arch', 'x86_64', '-framework', 'Cocoa', '-Wl,-undefined,dynamic_lookup' ])
	env.Append(RPATH=env.Literal("\\$$ORIGIN"))

if platform == 'android':
	android_arch = ARGUMENTS.get('android_arch', 'armv7')
	# Validate arch
	if android_arch not in ['armv7', 'arm64v8', 'x86', 'x86_64']:
		print("ERR: Invalid Android architecture \"", android_arch, "\"\nAllowed values: [armv7, arm64v8, x86, x86_64]")
		Exit(1)
	
	ndk_platform = int(ARGUMENTS.get('ndk_platform', 18 if android_arch in ['armv7', 'x86'] else 21))
	# Validate platform
	if ndk_platform < 21 and android_arch in ['arm64v8', 'x86_64']:
		print("WARN: ", android_arch, " architecture requires at least ndk_platform=21; settings ndk_platform=21")
		ndk_platform = 21
	
	ndk_root = ARGUMENTS.get('ANDROID_NDK_ROOT', os.environ.get('ANDROID_NDK_ROOT', None))
	# Validate NDK
	if not ndk_root:
		print("ERR: Could not find Android NDK path. Please install the Android NDK and set ANDROID_NDK_ROOT to the path to your installation.")
		Exit(1)
	
	# Get host platform
	if sys.platform.startswith("win"):
		host_platform = "windows"
		env = env.Clone(tools=['mingw'])
	elif sys.platform.startswith("linux"):
		host_platform = "linux"
	elif sys.platform.startswith("darwin"):
		host_platform = "darwin" # osx
	else:
		print("ERR: Could not detect host platform")
		print("sys.platform: ", sys.platform);
		Exit(1);
	
	toolchain = ndk_root + "/toolchains/llvm/prebuilt/" + host_platform
	import platform as pltfm
	if host_platform != 'windows' or pltfm.machine().endswith('64'):
		toolchain += '-x86_64'
	toolchain += "/bin/"

	env['CXX'] = toolchain + "clang++"
	env['CC'] = toolchain + "clang"

	# Setup arch-specifics
	if android_arch == 'armv7':
		target_flags = ['-target', 'armv7a-linux-androideabi' + str(ndk_platform), '-march=armv7-a', '-mfpu=neon']
		tool_prefix = "arm-linux-androideabi"
	elif android_arch == 'arm64v8':
		target_flags = ['-target', 'aarch64-linux-android' + str(ndk_platform), '-march=armv8-a']
		tool_prefix = "aarch64-linux-android"
	elif android_arch == "x86":
		target_flags = ['-target', 'i686-linux-android' + str(ndk_platform)]
		if ndk_platform < 24:
			env.Append(CCFLAGS=['-mstackrealign'])
		tool_prefix = "i686-linux-android"
	elif android_arch == "x86_64":
		target_flags = ['-target', 'x86_64-linux-android' + str(ndk_platform)]
		tool_prefix = "x86_64-linux-android"
	env.Append(CCFLAGS=target_flags, LINKFLAGS=target_flags)
	
	env['LD'] = toolchain + tool_prefix + "-ld"
	env['RANLIB'] = toolchain + tool_prefix + '-ranlib'
	env['AR'] = toolchain + tool_prefix + '-ar'
	env['AS'] = toolchain + tool_prefix + '-as'
	env['LINK'] = env['CXX']
	env.Append(CCFLAGS=['-fPIC'])
	env.Append(LINKFLAGS=['-fPIC'])
	env['SHLIBPREFIX'] = 'lib'
	env['SHLIBSUFFIX'] = '.so'

	def find_ixes_wrapper(self, paths, prefix, suffix): # Prevents bad linker flags
		if prefix == 'LIBPREFIX' and suffix == 'LIBSUFFIX':
			return False
		else:
			return self.old_ixes(paths, prefix, suffix)
	
	from types import MethodType
	env.old_ixes = env.FindIxes
	env.FindIxes = MethodType(find_ixes_wrapper, env)

	output += '.' + android_arch

godotcpp_lib += '.' + platform + '.' + target + '.' + (bits if platform != 'android' else android_arch)

if platform != 'android':
	if bits == '64':
		output += '.64';
	else:
		output += '.32';

# Include dir
env.Append(CPPPATH=[
	'src',
	'thirdparty/sqlite',
	'thirdparty/godot_cpp/godot_headers/',
	'thirdparty/godot_cpp/include',
	'thirdparty/godot_cpp/include/core',
	'thirdparty/godot_cpp/include/gen'
]);

# Source lists
sources = [
	'src/gdsqlite.cpp',
	'src/library.cpp',
	'thirdparty/sqlite/sqlite3.c',
	'thirdparty/sqlite/spmemvfs.c'
];

# Libraries
env.Append(LIBPATH=['thirdparty/godot_cpp/bin/']);
env.Append(LIBS=[godotcpp_lib]);

library = env.SharedLibrary(target=('bin/' + output + env['SHLIBSUFFIX']), source=sources);
if platform == 'android': # Fix non-existant outputs
	Install('demo/lib/gdsqlite', source=File('bin/' + env['SHLIBPREFIX'] + output + env['SHLIBSUFFIX']))
else:
	Install('demo/lib/gdsqlite', source=library);

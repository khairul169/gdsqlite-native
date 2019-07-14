# GDSQLite

[SQLite3](https://www.sqlite.org/) wrapper for Godot Engine 3.x

## Installation

Download latest [release](https://github.com/khairul169/gdsqlite-native/releases) of this module and uncompress it on your project directory.

## Example usage

- [SQL Queries](https://github.com/khairul169/gdsqlite-native/blob/master/demo/examples/sql_queries.gd)
- [Game Highscore](https://github.com/khairul169/gdsqlite-native/blob/master/demo/examples/game_highscore.gd)
- [Item Libraries](https://github.com/khairul169/gdsqlite-native/blob/master/demo/examples/item_database.gd)

## Compiling from source

Building GDSQLite from source requires the following:
- A C++ compiler for your system ([MSVC](https://visualstudio.microsoft.com/downloads/) for Windows, [gcc](https://stackoverflow.com/a/46254812) for Linux, [XCode](https://developer.apple.com/xcode/) for OSX)
- [Python](https://www.python.org/) (3.5 or higher, make sure to check "Add Python to PATH")
- [Git](https://git-scm.com/)

---
Open a command line window and enter the following:

```
python -m pip install scons
```

This will install Scons in your Python directory and hook it up to your PATH.

NOTE FOR OSX: You may need to install Scons like so:
```
brew install scons
```

---

Now, start a new command line prompt to refresh your PATH. `cd` to a directory on your computer where you want to put gdsqlite's source code and enter the following:
```
git clone --recursive https://github.com/khairul169/gdsqlite-native
```
This will download the source code to a new folder named `gdsqlite-native`, and initialize all of its submodules. `cd` into `gdsqlite-native/thirdparty/godot_cpp` and type the following:
```
scons platform=<platform> target=<target> bits=<bits> generate_bindings=yes
```
Where \<platform\> is the platform you are compiling for ('windows', 'linux', 'osx'), \<target\> is the compilation target type ('debug' or 'release'), and \<bits\> is the architecture to compile for (i.e. if you're targeting a 32-bit platform. put '32' for bits).<br/>
This command will generate the code bindings for the plugin to hook into Godot.

---

`cd` back up to `gdsqlite-native` (just enter `cd ../..`) and type the following:
```
scons platform=<platform> target=<target> bits=<bits>
```
Where platform, target and bits are the same configuration as what you put for `godot_cpp`. (Note: If you want a different configuration, you will have to rerun `scons` in `godot_cpp` with the configuration you need before you compile gdsqlite)<br/><br/>
Once that command is complete, the library will have been built under `gdsqlite-native/demo/lib`. Copy and paste that folder into your project directory. 
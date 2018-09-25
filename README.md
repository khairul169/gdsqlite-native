# GDSQLite

A [SQLite](https://www.sqlite.org/) wrapper for Godot Engine 3.0

## Installation

Download latest [release](https://github.com/khairul169/gdsqlite-native/releases) of this module and uncompress it on your project directory.

## Compiling for MacOS

1. Clone the repository, `cd` into the directory
2. `git submodule init`
3. `git submodule update`
4. `scons platform=osx`
5. `gdsqlite-native/godot_project/lib/gdsqlite/libgdsqlite.64.dylib` is generated. 
6. You now can run the test project in Godot 3.0 (tested on MacOS 10.14 in Godot 3.0.6).

## Example usage

- [SQL Queries](https://github.com/khairul169/gdsqlite-native/blob/master/godot_project/examples/sql_queries.gd)
- [Game Highscore](https://github.com/khairul169/gdsqlite-native/blob/master/godot_project/examples/game_highscore.gd)
- [Item Libraries](https://github.com/khairul169/gdsqlite-native/blob/master/godot_project/examples/item_database.gd)

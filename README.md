# GDSQLite

A [SQLite](https://www.sqlite.org/) wrapper for Godot Engine 3.0

## Installation

Download latest [release](https://github.com/khairul169/gdsqlite-native/releases) of this module and uncompress it on your project directory.

## How to compile

You will need [godot headers](https://github.com/GodotNativeTools/godot_headers) and compiled library of [c++ bindings](https://github.com/GodotNativeTools/cpp_bindings) before compiling this module.

**SConstruct:** set cpp_bindings and godot_headers to your own path.

**Depedencies:** scons, build package

Compiling on linux:
```
scons p=linux
```

Compiling on windows:
```
# windows x64
scons p=windows bits=64
# windows x86
scons p=windows bits=32
```

## Usage
```
extends Node

# Preload SQLite native script
var SQLite = preload("res://lib/sqlite.gdn");

# Instance our class
var db = SQLite.new();

func _ready():
	# Enable debugging
	db.set_debugging(true);
	
	# Open SQL
	if db.open("data.sql") != OK:
		print("ERR: ", db.get_errormsg());
		return;
	
	var columns = [];
	columns.append("id INTEGER PRIMARY KEY AUTOINCREMENT");
	columns.append("name TEXT");
	columns.append("age INTEGER");
	
	# args: String tablename | Array columns | Bool create only if not exists
	# return: Integer retval
	db.create_table("users", columns, true);
	
	# args: String query
	# return: Integer retval
	db.query("INSERT INTO users (name, age) VALUES ('khairul', '17');");
  
	# args: String query
	# return: Array rows
	var data = db.fetch_array("SELECT * FROM users LIMIT 10;");
	print("Rows count: ", data.size());
  
	for i in data:
		print(i);
  
	db.close();
```

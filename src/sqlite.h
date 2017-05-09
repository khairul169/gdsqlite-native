#ifndef GDSQLITE_H
#define GDSQLITE_H

#include "init.h"
#include "sqlite/sqlite3.h"

#include <Reference.hpp>
#include <OS.hpp>

class SQLite: public Reference {
	GODOT_CLASS(SQLite, Reference)

public:
	void set_debugging(bool debug);
	int open(Variant filename);
	void close();
	
	int create_table(Variant name, Variant columns, bool ifnotexists = 1);
	Array fetch_array(Variant query);
	int query(Variant query);
	String get_errormsg();
	
	static void _register_methods();

private:
	sqlite3 *db;
	sqlite3_stmt *stmt;
	
	bool _debugging = false;
	
	void prepare(const char* query);
	int step();
	Array fetch_assoc();
	void finalize();
};

#endif

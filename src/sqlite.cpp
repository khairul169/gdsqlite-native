#include "init.h"
#include "sqlite.h"

void SQLite::set_debugging(bool debug) {
	_debugging = debug;
}

int SQLite::open(Variant filename) {
	String path = OS::get_data_dir() + String("/") + filename;
	if (_debugging) printf("[SQLite] Database path: %s\n", path.c_string());
	return sqlite3_open(path.c_string(), &db);
}

void SQLite::prepare(const char* query) {
	if (_debugging) printf("[SQLite] Prepare query: %s\n", query);
	int ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (ret != SQLITE_OK && _debugging) printf("[SQLite] Error caught: %s\n", sqlite3_errmsg(db));
}

int SQLite::step() {
	return sqlite3_step(stmt);
}

Array SQLite::fetch_assoc() {
	Array result;
	
	while (1) {
		int ret = sqlite3_step(stmt);
		if (ret == SQLITE_ROW) {
			Dictionary row;
			for(unsigned int i = 0; i < sqlite3_column_count(stmt); ++i) {
				int type = sqlite3_column_type(stmt, i);
				switch (type) {
					case SQLITE_INTEGER: row[sqlite3_column_name(stmt,i)] = sqlite3_column_int(stmt, i); break;
					case SQLITE_FLOAT: row[sqlite3_column_name(stmt,i)] = sqlite3_column_double(stmt, i); break;
					case SQLITE_TEXT: row[sqlite3_column_name(stmt,i)] = String((char *)sqlite3_column_text(stmt, i)); break;
					default: break;
				}
			}
			
			result.push_back(row);
		}
		else if (ret == SQLITE_DONE) {
			break;
		}
		else {
			break;
		}
	}
	
	return result;
}

int SQLite::create_table(Variant name, Variant columns, bool ifnotexists) {
	char q[128];
	String c_name = name;
	sprintf(q, "CREATE TABLE %s`%s` (", (ifnotexists ? "IF NOT EXISTS ": ""), c_name.c_string());
	Array cols = columns;
	for (int i=0; i<cols.size(); i++) {
		String col = cols[i];
		sprintf(q, "%s %s %s", q, (i != 0 ? "," : ""), col.c_string());
	}
	sprintf(q, "%s);", q);
	prepare(q);
	int ret = step();
	finalize();
	return ret;
}

Array SQLite::fetch_array(Variant query) {
	String q = query;
	const char* cs = q.c_string();
	prepare(cs);
	Array result = fetch_assoc();
	finalize();
	return result;
}

int SQLite::query(Variant query) {
	String q = query;
	const char* cs = q.c_string();
	prepare(cs);
	int ret = step();
	finalize();
	return ret;
}

void SQLite::finalize() {
	sqlite3_finalize(stmt);
}

String SQLite::get_errormsg() {
	return String(sqlite3_errmsg(db));
}

void SQLite::close() {
	sqlite3_close(db);
}

void SQLite::_register_methods() {
	register_method((char *)"set_debugging", &SQLite::set_debugging);
	register_method((char *)"open", &SQLite::open);
	register_method((char *)"create_table", &SQLite::create_table);
	register_method((char *)"fetch_array", &SQLite::fetch_array);
	register_method((char *)"query", &SQLite::query);
	register_method((char *)"get_errormsg", &SQLite::get_errormsg);
	register_method((char *)"close", &SQLite::close);
}


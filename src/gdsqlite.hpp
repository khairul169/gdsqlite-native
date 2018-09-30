#ifndef GDSQLITE_H
#define GDSQLITE_H

#include <Godot.hpp>
#include <Reference.hpp>

#include <File.hpp>

// SQLite3
#include <sqlite3.h>
#include <spmemvfs.h>

#ifndef NULL
#define NULL 0
#endif

namespace godot {
	class SQLite : public Reference {
		GODOT_CLASS(SQLite, Reference)

	private:
		// sqlite handler
		sqlite3 *db;

		// vfs
		spmemvfs_db_t p_db;
		bool memory_read;

	public:
		enum {
			RESULT_BOTH = 0,
			RESULT_NUM,
			RESULT_ASSOC
		};

		static void _register_methods();

		// constructor
		SQLite();
		~SQLite();
		void _init() {}

		// methods
		bool open(String path);
		bool open_buffered(String name, PoolByteArray buffers, int64_t size);
		void close();
		bool backup(SQLite *other);

		bool query(String statement);
		Array fetch_array(String statement);
		Array fetch_assoc(String statement);

	private:
		sqlite3_stmt* prepare(const char* statement);
		Array fetch_rows(String query, int result_type = RESULT_BOTH);
		Dictionary parse_row(sqlite3_stmt *stmt, int result_type);
		sqlite3* get_handler() { return (memory_read ? p_db.handle : db); }
	};
}
#endif

#include "gdsqlite.h"
#include <string.h>

// Variables
sqlite3 *db = NULL;
sqlite3_stmt *stmt = NULL;

// For debugging purpose
bool debug_log = false;

// Native api
const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

// Singletons
godot_object *godot_globals = NULL;

/** Library entry point **/
void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o)
{
	// Core api
	api = o->api_struct;

	// Find nativescript api
	for (int i = 0; i < api->num_extensions; i++) {
		if (api->extensions[i]->type == GDNATIVE_EXT_NATIVESCRIPT)
			nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
	}
}

/** Library de-initialization **/
void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *o)
{
	api = NULL;
	nativescript_api = NULL;
}

// Constructor
void *gdsqlite_init(godot_object *p_instance, void *p_method_data) {
	printf("[SQLite] Module loaded.\n");
	return 0;
}

// Destructor
void gdsqlite_free(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	if (db != NULL) {
		sqlite3_close(db);
		db = NULL;
		stmt = NULL;
	}
}

char *alloc_c_string(godot_string *string) {
	godot_char_string contents = api->godot_string_utf8(string);

	int length = api->godot_char_string_length(&contents);
	char *result = (char *) api->godot_alloc(length + 1);

	if (result) {
		memcpy(result, api->godot_char_string_get_data(&contents), length + 1);
	}

	api->godot_char_string_destroy(&contents);
	return result;
}

// ProjectSettings.globalize_path(path)
const char *globalize_path(godot_string path) {
	if (godot_globals == NULL)
		godot_globals = api->godot_global_get_singleton((char *) "ProjectSettings");

	static godot_method_bind *mb = NULL;
	if (mb == NULL)
		mb = api->godot_method_bind_get_method("ProjectSettings", "globalize_path");

	// vars
	godot_string global_path;
	api->godot_string_new(&global_path);

	// arguments
	const void *args[] = {
		(void *) &path
	};

	// call method
	api->godot_method_bind_ptrcall(mb, godot_globals, args, &global_path);

	// return user dir
	char *ret;
	ret = alloc_c_string(&global_path);
	return ret;
}

// Open & load database
godot_variant sqlite_open(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args) {
	godot_variant ret;

	// Database is not null or no argument supplied.
	if (db != NULL || num_args < 1) {
		api->godot_variant_new_bool(&ret, false);
		return ret;
	}

	// Parse filename argument
	godot_string fname = api->godot_variant_as_string(args[0]);

	// Database file path
	const char *path = globalize_path(fname);

	// Open database
	int err = sqlite3_open(path, &db);
	api->godot_variant_new_bool(&ret, (err != SQLITE_OK) ? false : true);
	return ret;
}

int sqlite_prepare(const char* query) {
	int ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (ret != SQLITE_OK)
		printf("[SQLite] Error: %s\n", sqlite3_errmsg(db));
	return ret;
}

godot_variant sqlite_query(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args) {
	godot_variant ret;

	// No args
	if (num_args < 1) {
		api->godot_variant_new_bool(&ret, false);
		return ret;
	}

	// Parse arguments
	godot_string arg_query = api->godot_variant_as_string(args[0]);
	int retv = sqlite_prepare(alloc_c_string(&arg_query));

	// Failed to prepare the query
	if (retv != SQLITE_OK)
	{
		api->godot_variant_new_bool(&ret, false);
		return ret;
	}

	// Evaluate the sql query
	retv = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	api->godot_variant_new_bool(&ret, true);
	return ret;
}

godot_variant sqlite_fetch_array(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args) {
	godot_variant ret;
	api->godot_variant_new_nil(&ret);

	// No args
	if (num_args < 1)
		return ret;
	
	// Parse arguments
	godot_string arg_query = api->godot_variant_as_string(args[0]);
	int retv = sqlite_prepare(alloc_c_string(&arg_query));

	// Failed to prepare the query
	if (retv != SQLITE_OK)
		return ret;
	
	// The rows handler
	godot_array rows;
	api->godot_array_new(&rows);

	// Fetch rows
	while (true) {
		retv = sqlite3_step(stmt);

		// Row fetched
		if (retv == SQLITE_ROW) {
			godot_dictionary row;
			api->godot_dictionary_new(&row);

			for (unsigned int i = 0; i < sqlite3_column_count(stmt); ++i) {
				godot_variant key;
				godot_variant value;

				// Dictionary key
				const char *colname = sqlite3_column_name(stmt, i);
				godot_string key_string;
				api->godot_string_new(&key_string);
				api->godot_string_parse_utf8(&key_string, colname);
				api->godot_variant_new_string(&key, &key_string);

				// Value datatype
				int type = sqlite3_column_type(stmt, i);

				if (type == SQLITE_INTEGER) {
					int val = sqlite3_column_int(stmt, i);
					api->godot_variant_new_int(&value, val);
					api->godot_dictionary_set(&row, &key, &value);
				}

				if (type == SQLITE_FLOAT) {
					double val = sqlite3_column_double(stmt, i);
					api->godot_variant_new_real(&value, val);
					api->godot_dictionary_set(&row, &key, &value);
				}

				if (type == SQLITE_TEXT) {
					const char* val = sqlite3_column_text(stmt, i);
					godot_string s;
					api->godot_string_new(&s);
					api->godot_string_parse_utf8(&s, val);
					api->godot_variant_new_string(&value, &s);
					api->godot_dictionary_set(&row, &key, &value);
				}
			}

			godot_variant v;
			api->godot_variant_new_dictionary(&v, &row);
			api->godot_array_append(&rows, &v);
		}

		else break;
	}

	// Destroy data
	sqlite3_finalize(stmt);

	// Return data
	api->godot_variant_destroy(&ret);
	api->godot_variant_new_array(&ret, &rows);
	return ret;
}

// Open & load database
godot_variant sqlite_close(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args) {
	godot_variant ret;

	// No database loaded
	if (!db || sqlite3_close_v2(db) != SQLITE_OK) {
		api->godot_variant_new_bool(&ret, false);
		return ret;
	}

	db = NULL;

	// return OK
	api->godot_variant_new_bool(&ret, true);
	return ret;
}

// Check if database are loaded
godot_variant sqlite_loaded(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args) {
	godot_variant ret;

	// Database is not null
	if (db != NULL) {
		api->godot_variant_new_bool(&ret, true);
		return ret;
	}

	// return false
	api->godot_variant_new_bool(&ret, false);
	return ret;
}

godot_variant sqlite_last_insert_rowid(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args) {
	godot_variant ret;
	if (!db) {
		api->godot_variant_new_int(&ret, 0);
		return ret;
	}

	int rowid = (int)sqlite3_last_insert_rowid(db);

	// return false
	api->godot_variant_new_int(&ret, rowid);
	return ret;
}

void register_method(void* handle, const char* method_name, void* method_ptr)
{
	godot_instance_method method_instance;
	method_instance.method = method_ptr;

	godot_method_attributes attrib;
	attrib.rpc_type = GODOT_METHOD_RPC_MODE_DISABLED;

	nativescript_api->godot_nativescript_register_method(handle, SQLITE_CLASSNAME, method_name, attrib, method_instance);
}

/** Script entry (Registering all the classes and stuff) **/
void GDN_EXPORT godot_nativescript_init(void *p_handle)
{
	// Contructor & destructor
	godot_instance_create_func create = { NULL, NULL, NULL };
	create.create_func = &gdsqlite_init;

	godot_instance_destroy_func destroy = { NULL, NULL, NULL };
	destroy.destroy_func = &gdsqlite_free;

	// Register class
	nativescript_api->godot_nativescript_register_class(p_handle, SQLITE_CLASSNAME, SQLITE_BASECLASS, create, destroy);

	// Methods
	register_method(p_handle, "open_db", &sqlite_open);
	register_method(p_handle, "query", &sqlite_query);
	register_method(p_handle, "fetch_array", &sqlite_fetch_array);
	register_method(p_handle, "close", &sqlite_close);
	register_method(p_handle, "loaded", &sqlite_loaded);
	register_method(p_handle, "last_insert_rowid", &sqlite_last_insert_rowid);
}

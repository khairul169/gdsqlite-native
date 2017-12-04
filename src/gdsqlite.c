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
godot_object *godot_os = NULL;
godot_method_bind *godot_os_getdatadir = NULL;

/** Library entry point **/
void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *o)
{
	// Core api
	api = o->api_struct;

	// Find nativescript api
	for (int i = 0; i < api->num_extensions; i++) {
		if (api->extensions[i]->type == GDNATIVE_EXT_NATIVESCRIPT)
		{
			nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
		}
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
	printf("GDSQLite loaded.\n");
	return 0;
}

// Destructor
void gdsqlite_free(godot_object *p_instance, void *p_method_data, void *p_user_data) {
	if (db != NULL)
	{
		sqlite3_close(db);
		db = NULL;
		stmt = NULL;
	}
	
	printf("GDSQLite unloaded.\n");
}

char *godot_string_to_char(godot_string *str)
{
	int len;

	// figure out the lenght of our string
	api->godot_string_get_data(str, NULL, &len);

	// allocate our buffer
	char *result = (char *)api->godot_alloc(len + 1);
	if (result != NULL) {
		api->godot_string_get_data(str, result, &len);
		result[len] = '\0';
	}

	return result;
}

// Get user data directory
char *get_data_dir()
{
	// Load OS singleton
	if (godot_os == NULL)
		godot_os = api->godot_global_get_singleton((char *) "OS");
	if (godot_os_getdatadir == NULL)
		godot_os_getdatadir = api->godot_method_bind_get_method("_OS", "get_user_data_dir");

	// vars
	godot_string usr_dir;
	api->godot_string_new(&usr_dir);
	const void *args[1];

	// call method
	api->godot_method_bind_ptrcall(godot_os_getdatadir, godot_os, args, &usr_dir);

	// return user dir
	char *ret;
	ret = godot_string_to_char(&usr_dir);
	return ret;
}

// Open & load database
godot_variant sqlite_open(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args)
{
	godot_variant ret;

	// Database is not null or no argument supplied.
	if (db != NULL || num_args < 1)
	{
		api->godot_variant_new_int(&ret, -1);
		return ret;
	}

	// Parse filename argument
	godot_string fname = api->godot_variant_as_string(args[0]);

	// Database file path
	char *filename, filepath[256];
	filename = godot_string_to_char(&fname);

	strcpy(filepath, get_data_dir());
	strcat(filepath, "/");
	strcat(filepath, filename);
	
	printf("[SQLite] Database path: res://%s\n", filename);

	// Open database
	int err = sqlite3_open(filepath, &db);

	api->godot_variant_new_int(&ret, err);
	return ret;
}

int sqlite_prepare(const char* query)
{
	int ret = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (ret != SQLITE_OK)
		printf("[SQLite] Error: %s\n", sqlite3_errmsg(db));
	return ret;
}

godot_variant sqlite_query(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args)
{
	godot_variant ret;

	// No args
	if (num_args < 1)
	{
		api->godot_variant_new_int(&ret, -1);
		return ret;
	}

	// Parse arguments
	godot_string arg_query = api->godot_variant_as_string(args[0]);
	int retv = sqlite_prepare(godot_string_to_char(&arg_query));

	// Failed to prepare the query
	if (retv != SQLITE_OK)
	{
		api->godot_variant_new_int(&ret, -1);
		return ret;
	}

	// Evaluate the sql query
	retv = sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	api->godot_variant_new_int(&ret, retv);
	return ret;
}

godot_variant sqlite_fetch_array(godot_object *obj, void *method_data, void *user_data, int num_args, godot_variant **args)
{
	godot_variant ret;
	api->godot_variant_new_nil(&ret);

	// No args
	if (num_args < 1)
		return ret;
	
	// Parse arguments
	godot_string arg_query = api->godot_variant_as_string(args[0]);
	int retv = sqlite_prepare(godot_string_to_char(&arg_query));

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
				api->godot_string_new_data(&key_string, colname, strlen(colname));
				api->godot_variant_new_string(&key, &key_string);

				// Value datatype
				int type = sqlite3_column_type(stmt, i);

				if (type == SQLITE_INTEGER)
				{
					int val = sqlite3_column_int(stmt, i);
					api->godot_variant_new_int(&value, val);
					api->godot_dictionary_set(&row, &key, &value);
				}

				if (type == SQLITE_FLOAT)
				{
					double val = sqlite3_column_double(stmt, i);
					api->godot_variant_new_real(&value, val);
					api->godot_dictionary_set(&row, &key, &value);
				}

				if (type == SQLITE_TEXT)
				{
					const char* val = sqlite3_column_text(stmt, i);
					godot_string s;
					api->godot_string_new_data(&s, val, strlen(val));
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
	sqlite3_finalize(stmt);

	// Return data
	api->godot_variant_destroy(&ret);
	api->godot_variant_new_array(&ret, &rows);
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
	register_method(p_handle, "open_database", &sqlite_open);
	register_method(p_handle, "query", &sqlite_query);
	register_method(p_handle, "fetch_array", &sqlite_fetch_array);
}

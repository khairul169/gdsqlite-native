#include "init.h"
#include "sqlite.h"

GODOT_NATIVE_INIT(godot_native_init_options *options) {
    register_class<SQLite>();
}

GODOT_NATIVE_TERMINATE(godot_native_terminate_options *options) {
}


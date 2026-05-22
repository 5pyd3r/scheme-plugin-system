#include "plugin.h"
#include "chez_adaptor.h"
#include <stdio.h>

int plugin_system_init(const char *boot_path) {
    if (chez_init(boot_path) != 0) {
        fprintf(stderr, "plugin_system_init: chez_init failed\n");
        return -1;
    }
    return 0;
}

int plugin_register_c_func(const char *name, plugin_c_func fn) {
    if (chez_register_foreign(name, (void*)fn) != 0) {
        fprintf(stderr, "plugin_register_c_func: failed for %s\n", name);
        return -1;
    }
    return 0;
}

int plugin_load(const char *path) {
    if (chez_load_file(path) != 0) {
        fprintf(stderr, "plugin_load: failed to load %s\n", path);
        return -1;
    }
    return 0;
}

void* plugin_call(const char *name) {
    void *result = chez_call(name);
    if (!result) {
        fprintf(stderr, "plugin_call: failed for %s\n", name);
    }
    return result;
}

void plugin_system_destroy(void) {
    chez_destroy();
}

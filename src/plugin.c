#include "plugin.h"
#include "chez_adaptor.h"
#include <stdio.h>

/* By default, init in runtime mode (petite.boot only).
 * For dev mode (compiler support), call plugin_system_init_dev() instead,
 * or build with -DPLUGIN_DEV and call plugin_system_init() with a boot_dir
 * that contains scheme.boot. */

static int is_dev = 0;

int plugin_system_init(const char *boot_dir) {
    /* Default: runtime mode. Dev mode is selected at compile time
     * via #ifdef PLUGIN_DEV in plugin.h */
    if (chez_init_runtime(boot_dir) != 0) {
        fprintf(stderr, "plugin_system_init: chez_init_runtime failed\n");
        return -1;
    }
    return 0;
}

/* Dev-mode init — loads compiler boot file */
int plugin_system_init_dev(const char *boot_dir) {
    if (chez_init_dev(boot_dir) != 0) {
        fprintf(stderr, "plugin_system_init_dev: chez_init_dev failed\n");
        return -1;
    }
    is_dev = 1;
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

#ifdef PLUGIN_DEV
int plugin_compile(const char *src_path, const char *output_path) {
    if (!is_dev) {
        fprintf(stderr, "plugin_compile: requires dev environment. "
                        "Use plugin_system_init_dev() instead of plugin_system_init().\n");
        return -1;
    }
    if (chez_compile_file(src_path, output_path) != 0) {
        fprintf(stderr, "plugin_compile: failed to compile %s\n", src_path);
        return -1;
    }
    printf("[Dev] Compiled %s → %s\n", src_path, output_path);
    return 0;
}
#endif

void plugin_system_destroy(void) {
    chez_destroy();
    is_dev = 0;
}

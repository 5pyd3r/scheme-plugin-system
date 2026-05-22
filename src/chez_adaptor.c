#include "chez_adaptor.h"
#include "scheme.h"
#include <stdio.h>
#include <string.h>

static int initialized = 0;
static int dev_mode = 0;

/* Internal: register and load boot files, then init kernel. */
static int chez_init_internal(const char *boot_dir, int load_compiler) {
    if (initialized) return -1;

    Sscheme_init(NULL);

    char path[2048];

    /* Always load petite.boot (interpreter) */
    snprintf(path, sizeof(path), "%s/petite.boot", boot_dir);
    Sregister_boot_file(path);

    /* Optionally load scheme.boot (compiler) */
    if (load_compiler) {
        snprintf(path, sizeof(path), "%s/scheme.boot", boot_dir);
        Sregister_boot_file(path);
    }

    Sbuild_heap(NULL, NULL);

    initialized = 1;
    dev_mode = load_compiler;
    return 0;
}

int chez_init_runtime(const char *boot_dir) {
    return chez_init_internal(boot_dir, 0);
}

int chez_init_dev(const char *boot_dir) {
    return chez_init_internal(boot_dir, 1);
}

int chez_register_foreign(const char *name, void *fn) {
    if (!initialized) return -1;
    Sforeign_symbol(name, fn);
    return 0;
}

int chez_load_file(const char *path) {
    if (!initialized) return -1;

    ptr load_proc = Stop_level_value(Sstring_to_symbol("load"));
    if (!load_proc) {
        fprintf(stderr, "chez_load_file: 'load' not found\n");
        return -1;
    }

    ptr path_str = Sstring(path);
    Scall1(load_proc, path_str);
    return 0;
}

void* chez_call(const char *name) {
    if (!initialized) return NULL;

    ptr sym = Sstring_to_symbol(name);
    if (!sym) {
        fprintf(stderr, "chez_call: symbol '%s' not found\n", name);
        return NULL;
    }

    ptr proc = Stop_level_value(sym);
    if (!proc) {
        fprintf(stderr, "chez_call: procedure '%s' not bound\n", name);
        return NULL;
    }

    return (void*)Scall0(proc);
}

int chez_compile_file(const char *src_path, const char *output_path) {
    if (!initialized) return -1;
    if (!dev_mode) {
        fprintf(stderr, "chez_compile_file: requires dev environment (scheme.boot)\n");
        return -1;
    }

    /* Call Chez Scheme's compile-file to produce a native .so.
     * (compile-file "src" "output") — both args are strings. */
    ptr compile_proc = Stop_level_value(Sstring_to_symbol("compile-file"));
    if (!compile_proc) {
        fprintf(stderr, "chez_compile_file: 'compile-file' not available\n");
        return -1;
    }

    ptr src_str = Sstring(src_path);
    ptr out_str = Sstring(output_path);
    Scall2(compile_proc, src_str, out_str);
    return 0;
}

void chez_destroy(void) {
    if (!initialized) return;
    Sscheme_deinit();
    initialized = 0;
    dev_mode = 0;
}

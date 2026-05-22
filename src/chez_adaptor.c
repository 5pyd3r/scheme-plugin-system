#include "chez_adaptor.h"
#include "scheme.h"
#include <stdio.h>

static int initialized = 0;

int chez_init(const char *boot_path) {
    if (initialized) return -1;

    Sscheme_init(NULL);

    /* Register both petite.boot (interpreter) and scheme.boot (compiler).
     * petite.boot provides the base Scheme runtime;
     * scheme.boot provides the compiler needed for foreign-procedure.
     * The boot_path is expected to be the directory containing both files. */
    char petite_path[1024], scheme_path[1024];
    snprintf(petite_path, sizeof(petite_path), "%s/petite.boot", boot_path);
    snprintf(scheme_path, sizeof(scheme_path), "%s/scheme.boot", boot_path);

    Sregister_boot_file(petite_path);
    Sregister_boot_file(scheme_path);
    Sbuild_heap(NULL, NULL);

    initialized = 1;
    return 0;
}

int chez_register_foreign(const char *name, void *fn) {
    if (!initialized) return -1;

    /* Register the C function pointer in Chez Scheme's foreign symbol table.
     * Scheme code can then access it via (foreign-procedure "name" ...) or
     * the convenience wrapper we install below. */
    Sforeign_symbol(name, fn);

    /* Also make it directly callable as a Scheme procedure.
     * This creates a top-level binding so (host-multiply) works.
     * We construct and eval: (define <name> (foreign-procedure "<name>"))
     *
     * Sforeign_callable_entry_point can be used, but the simpler approach
     * is to register via Sforeign_symbol and then eval a wrapper.
     * For now, just the Sforeign_symbol registration is done —
     * the Scheme plugin must use (foreign-procedure "<name>") explicitly.
     */

    return 0;
}

int chez_load_file(const char *path) {
    if (!initialized) return -1;

    /* Call Scheme's (load "path") function */
    ptr load_proc = Stop_level_value(Sstring_to_symbol("load"));
    ptr path_str = Sstring(path);

    if (!load_proc) {
        fprintf(stderr, "chez_load_file: 'load' not found\n");
        return -1;
    }

    Scall1(load_proc, path_str);
    return 0;
}

void* chez_call(const char *name) {
    if (!initialized) return NULL;

    /* Look up the Scheme function by name and call it with 0 args */
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

    /* Check if the value is actually a procedure */
    ptr result = Scall0(proc);
    return (void*)result;
}

void chez_destroy(void) {
    if (!initialized) return;
    Sscheme_deinit();
    initialized = 0;
}

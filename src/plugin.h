#ifndef PLUGIN_H
#define PLUGIN_H

/* Opaque handle for Scheme objects. Callers treat as void*. */
typedef void* (*plugin_c_func)(void *arg);

/*
 * RUNTIME API — available in both dev and runtime environments.
 * Requires only petite.boot (interpreter).
 */

/* Initialize the plugin system in runtime mode.
 * boot_dir: directory containing petite.boot (and optionally scheme.boot). */
int plugin_system_init(const char *boot_dir);

/* Register a C function callable from Scheme as `name`. */
int plugin_register_c_func(const char *name, plugin_c_func fn);

/* Load and evaluate a plugin file (.ss source or pre-compiled .so). */
int plugin_load(const char *path);

/* Call a Scheme function by name, return result as opaque handle. */
void* plugin_call(const char *name);

/* Shutdown and free resources. */
void plugin_system_destroy(void);

/*
 * DEV API — additional functions available only in the dev environment.
 * Requires scheme.boot (compiler) in addition to petite.boot.
 */

/* Initialize the plugin system in dev mode (with compiler).
 * boot_dir must contain both petite.boot and scheme.boot. */
int plugin_system_init_dev(const char *boot_dir);

#ifdef PLUGIN_DEV

/* Compile a Scheme source file to a native shared object.
 * src_path: .ss/.scm source file
 * output_path: destination .so file
 * Returns 0 on success, -1 on error. */
int plugin_compile(const char *src_path, const char *output_path);

#endif /* PLUGIN_DEV */

#endif /* PLUGIN_H */

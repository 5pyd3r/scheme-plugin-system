#ifndef PLUGIN_H
#define PLUGIN_H

/* Opaque handle for Scheme objects. Callers treat as void*. */
typedef void* (*plugin_c_func)(void *arg);

/* Initialize the plugin system. boot_path points to petite.boot. */
int plugin_system_init(const char *boot_path);

/* Register a C function callable from Scheme as `name`. */
int plugin_register_c_func(const char *name, plugin_c_func fn);

/* Load and evaluate a Scheme plugin file. */
int plugin_load(const char *path);

/* Call a Scheme function by name, return result as opaque handle. */
void* plugin_call(const char *name);

/* Shutdown and free resources. */
void plugin_system_destroy(void);

#endif /* PLUGIN_H */

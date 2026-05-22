#ifndef CHEZ_ADAPTOR_H
#define CHEZ_ADAPTOR_H

/*
 * Runtime mode: loads only petite.boot (interpreter).
 * Dev mode: loads petite.boot + scheme.boot (interpreter + compiler).
 */

/* Initialize in runtime mode (petite.boot only). */
int chez_init_runtime(const char *boot_dir);

/* Initialize in dev mode (petite.boot + scheme.boot). */
int chez_init_dev(const char *boot_dir);

/* Register a C function as a Scheme-callable foreign procedure. */
int chez_register_foreign(const char *name, void *fn);

/* Load a Scheme source file or pre-compiled .so. */
int chez_load_file(const char *path);

/* Call a Scheme procedure by name, return result as ptr. */
void* chez_call(const char *name);

/* Compile a Scheme source file to a native .so (dev mode only). */
int chez_compile_file(const char *src_path, const char *output_path);

/* Shutdown Chez Scheme. */
void chez_destroy(void);

#endif

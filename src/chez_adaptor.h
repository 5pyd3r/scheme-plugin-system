#ifndef CHEZ_ADAPTOR_H
#define CHEZ_ADAPTOR_H

/* Initialize Chez Scheme kernel with given boot file. */
int chez_init(const char *boot_path);

/* Register a C function as a Scheme-callable foreign procedure. */
int chez_register_foreign(const char *name, void *fn);

/* Load a Scheme source file. */
int chez_load_file(const char *path);

/* Call a Scheme procedure by name, return the result as a ptr. */
void* chez_call(const char *name);

/* Shutdown Chez Scheme. */
void chez_destroy(void);

#endif

#include "plugin.h"
#include "scheme.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* C function exposed to Scheme via Sforeign_symbol.
 * Returns int directly — foreign-procedure declares integer-32 return type. */
static int c_multiply(void) {
    printf("[C] host-multiply called, returning 42\n");
    return 42;
}

static const char *BOOT_DIR = "/app/scheme-plugin-system/chezscheme/boot/tarm64le";

static void demo_runtime(void) {
    printf("=== RUNTIME ENVIRONMENT ===\n");
    printf("(petite.boot only — interpreter, no compiler)\n\n");

    /* Init runtime — uses only petite.boot */
    printf("[C] Initializing runtime...\n");
    if (plugin_system_init(BOOT_DIR) != 0) {
        fprintf(stderr, "Runtime init failed\n");
        return;
    }

    /* Register C function */
    plugin_register_c_func("host_multiply", (plugin_c_func)c_multiply);

    /* Load pre-compiled plugin */
    printf("[C] Loading compiled plugin (hello_plugin.so)...\n");
    if (plugin_load("plugins/hello_plugin.so") == 0) {
        ptr result = (ptr)plugin_call("get-result");
        printf("[C] Runtime: get-result = %ld\n", (long)Sfixnum_value(result));
    } else {
        /* Fallback: load source */
        printf("[C] Loading source plugin (hello_plugin.ss)...\n");
        plugin_load("plugins/hello_plugin.ss");
        ptr result = (ptr)plugin_call("get-result");
        printf("[C] Runtime: get-result = %ld\n", (long)Sfixnum_value(result));
    }

    plugin_system_destroy();
}

static void demo_compile(void) {
    printf("\n=== DEV ENVIRONMENT ===\n");
    printf("(petite.boot + scheme.boot — interpreter + compiler)\n\n");

    /* Init dev — uses petite.boot + scheme.boot */
    printf("[Dev] Initializing dev environment...\n");
    if (plugin_system_init_dev(BOOT_DIR) != 0) {
        fprintf(stderr, "Dev init failed\n");
        return;
    }

    /* Compile source to .so */
    printf("[Dev] Compiling hello_plugin.ss → hello_plugin.so...\n");
    if (plugin_compile("plugins/hello_plugin.ss",
                       "plugins/hello_plugin.so") != 0) {
        fprintf(stderr, "Compile failed\n");
        plugin_system_destroy();
        return;
    }

    printf("[Dev] Compilation successful.\n");
    plugin_system_destroy();

    /* Now load the compiled .so in runtime mode */
    printf("\n[Dev] Verifying compiled plugin in runtime...\n");
    if (plugin_system_init(BOOT_DIR) != 0) {
        fprintf(stderr, "Runtime init failed\n");
        return;
    }
    plugin_register_c_func("host_multiply", (plugin_c_func)c_multiply);

    printf("[Dev] Loading compiled hello_plugin.so...\n");
    plugin_load("plugins/hello_plugin.so");
    ptr result = (ptr)plugin_call("get-result");
    printf("[Dev] Runtime verified: get-result = %ld\n", (long)Sfixnum_value(result));

    plugin_system_destroy();
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "compile") == 0) {
        demo_compile();
    } else {
        demo_runtime();
    }
    printf("\n[C] Demo complete.\n");
    return 0;
}

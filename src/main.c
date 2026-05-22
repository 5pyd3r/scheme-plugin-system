#include "plugin.h"
#include "scheme.h"
#include <stdio.h>
#include <stdlib.h>

/* C function exposed to Scheme via Sforeign_symbol.
 * Scheme calls it as (host-multiply) → returns 42. */
static ptr c_multiply(void) {
    printf("[C] host-multiply called, returning 42\n");
    return Sinteger(42);
}

int main(int argc, char **argv) {
    const char *boot_path = "chezscheme/boot/tarm64le/petite.boot";

    printf("=== Scheme Plugin System Demo ===\n\n");

    /* Step 1: Initialize Chez Scheme kernel */
    printf("[C] Initializing Chez Scheme kernel...\n");
    if (plugin_system_init(boot_path) != 0) {
        fprintf(stderr, "Failed to initialize plugin system\n");
        return 1;
    }

    /* Step 2: Register C function for Scheme to call */
    printf("[C] Registering host-multiply...\n");
    if (plugin_register_c_func("host_multiply", (plugin_c_func)c_multiply) != 0) {
        fprintf(stderr, "Failed to register C function\n");
        return 1;
    }

    /* Step 3: Load Scheme plugin file */
    printf("[C] Loading hello_plugin.ss...\n");
    if (plugin_load("plugins/hello_plugin.ss") != 0) {
        fprintf(stderr, "Failed to load plugin\n");
        return 1;
    }

    /* Step 4: Call Scheme function get-result */
    printf("[C] Calling Scheme get-result...\n");
    ptr scheme_result = (ptr)plugin_call("get-result");
    iptr val = Sinteger_value(scheme_result);
    printf("[C] Scheme get-result returned: %ld\n", (long)val);

    /* Step 5: Cleanup */
    plugin_system_destroy();
    printf("\n[C] Demo complete.\n");
    return 0;
}

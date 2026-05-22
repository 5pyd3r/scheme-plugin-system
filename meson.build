project('scheme-plugin-system', 'c')

fs = import('fs')

# Detect Chez Scheme architecture directory
arch_dir = ''
foreach d : ['tarm64le', 'a6le', 'i3le', 'arm32le', 'ta6le', 'ti3le']
  lib = 'chezscheme' / d / 'boot' / d / 'libkernel.a'
  if fs.exists(lib)
    arch_dir = d
  endif
endforeach

if arch_dir == ''
  error('Chez Scheme not built. Run: cd chezscheme && ./configure --disable-x11 --disable-curses && make')
endif

message('Chez Scheme architecture: ' + arch_dir)

chez_base = meson.project_source_root() / 'chezscheme'
chez_boot = chez_base / arch_dir / 'boot' / arch_dir
chez_zlib = chez_base / arch_dir / 'zlib'
chez_lz4  = chez_base / arch_dir / 'lz4' / 'lib'

# Common include paths for Chez Scheme headers
chez_includes = [
  include_directories('chezscheme' / 'c'),
  include_directories('chezscheme' / 'boot' / arch_dir),
]

# Chez Scheme static libs + system libs (PIC compiled, linkable into .so)
chez_link_args = [
  chez_boot / 'libkernel.a',
  chez_zlib / 'libz.a',
  chez_lz4 / 'liblz4.a',
  '-lm', '-ldl', '-lpthread',
]

# ---- Shared Libraries ----

# Runtime shared library: petite.boot only (interpreter, no compiler)
libschemeplugin = shared_library('schemeplugin',
  'src/plugin.c',
  'src/chez_adaptor.c',
  include_directories: chez_includes,
  link_args: chez_link_args,
  c_args: ['-DPLUGIN_SYSTEM'],
  install: true,
)

# Dev shared library: petite.boot + scheme.boot (interpreter + compiler)
libschemeplugin_dev = shared_library('schemeplugin-dev',
  'src/plugin.c',
  'src/chez_adaptor.c',
  include_directories: chez_includes,
  link_args: chez_link_args,
  c_args: ['-DPLUGIN_SYSTEM', '-DPLUGIN_DEV'],
  install: true,
)

# ---- Demo Executable ----

# Demo host: links against the dev library (supports both --runtime and --dev modes)
executable('scheme-plugin-host',
  'src/main.c',
  link_with: [libschemeplugin_dev],
  include_directories: [
    include_directories('src'),
    chez_includes,
  ],
  c_args: ['-DPLUGIN_SYSTEM', '-DPLUGIN_DEV'],
  install: false,
)

# ---- Hello plugin pre-compiled (.so) for runtime demo ----

# Write a wrapper to call compile-file from chez scheme to generate hello_plugin.so
# This is done at build time: use the built scheme to compile the plugin
hello_plugin_ss = meson.project_source_root() / 'plugins' / 'hello_plugin.ss'
hello_plugin_so = meson.project_build_root() / 'plugins' / 'hello_plugin.so'

run_target('compile-plugin',
  command: [
    'sh', '-c',
    'mkdir -p "$(dirname "$2")" && ' +
    'LD_LIBRARY_PATH="$(dirname "$1")" ' +
    '"$(dirname "$1")/scheme-plugin-host" compile',
    'placeholder',  # $0
    meson.project_build_root(),  # $1
    hello_plugin_so,  # $2
  ],
)

# ---- Packaging ----

pkg_dir_release  = meson.project_build_root() / 'release'
boot_dir         = meson.project_source_root() / 'chezscheme' / 'boot' / arch_dir
src_dir          = meson.project_source_root() / 'src'

run_target('package',
  command: [
    'sh', '-c',
    'rm -rf "$0/runtime" "$0/dev" && ' +
    # Runtime package
    'mkdir -p "$0/runtime/boot" && ' +
    'cp "$1/libschemeplugin.so" "$0/runtime/" && ' +
    'cp "$2/plugin.h" "$0/runtime/" && ' +
    'cp "$3/petite.boot" "$0/runtime/boot/" && ' +
    'cp "$4/README.md" "$0/runtime/" && ' +
    # Dev package
    'mkdir -p "$0/dev/boot" && ' +
    'cp "$1/libschemeplugin-dev.so" "$0/dev/libschemeplugin.so" && ' +
    'cp "$2/plugin.h" "$0/dev/" && ' +
    'cp "$3/petite.boot" "$0/dev/boot/" && ' +
    'cp "$3/scheme.boot" "$0/dev/boot/" && ' +
    'cp "$4/README.md" "$0/dev/" && ' +
    'echo "=== Runtime Package ===" && ' +
    'find "$0/runtime" -type f | sort && ' +
    'echo "" && ' +
    'echo "=== Dev Package ===" && ' +
    'find "$0/dev" -type f | sort',
    pkg_dir_release,
    meson.project_build_root(),
    src_dir,
    boot_dir,
    meson.project_source_root(),
  ],
)

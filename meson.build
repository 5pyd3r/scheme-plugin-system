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

# Chez Scheme as an external dependency (statically linked)
chez_dep = declare_dependency(
  include_directories: [
    include_directories('chezscheme' / 'c'),
    include_directories('chezscheme' / 'boot' / arch_dir),
  ],
  link_args: [
    chez_boot / 'libkernel.a',
    chez_zlib / 'libz.a',
    chez_lz4 / 'liblz4.a',
    '-lm', '-ldl', '-lpthread',
  ],
)

# Shared library: libschemeplugin.so
# Contains plugin API + Chez Scheme kernel (statically linked inside).
# This is the distributable artifact — users link against it.
libschemeplugin = shared_library('schemeplugin',
  'src/plugin.c',
  'src/chez_adaptor.c',
  dependencies: [chez_dep],
  c_args: ['-DPLUGIN_SYSTEM'],
  install: true,
)

# Demo host: links against the shared library
executable('scheme-plugin-host',
  'src/main.c',
  link_with: [libschemeplugin],
  include_directories: [
    include_directories('src'),
    include_directories('chezscheme' / 'c'),
    include_directories('chezscheme' / 'boot' / arch_dir),
  ],
  c_args: ['-DPLUGIN_SYSTEM'],
  install: false,
)

# ---- Packaging ----

boot_dir = meson.project_source_root() / 'chezscheme' / 'boot' / arch_dir
src_dir  = meson.project_source_root() / 'src'
build_root = meson.project_build_root()

# Collect release files:
#   libschemeplugin.so  - the plugin system shared library
#   plugin.h            - public API header
#   petite.boot         - Chez Scheme interpreter runtime
#   scheme.boot         - Chez Scheme compiler runtime
#   README.md           - documentation
pkg_dir = build_root / 'release' / 'schemeplugin'

run_target('package',
  command: [
    'sh', '-c',
    'rm -rf "$0" && ' +
    'mkdir -p "$0/boot" && ' +
    'cp "$1/libschemeplugin.so" "$0/" && ' +
    'cp "$2/plugin.h" "$0/" && ' +
    'cp "$3/petite.boot" "$0/boot/" && ' +
    'cp "$3/scheme.boot" "$0/boot/" && ' +
    'cp "$4/README.md" "$0/" && ' +
    'echo "Release package: $0" && ' +
    'find "$0" -type f | sort',
    pkg_dir,
    build_root,
    src_dir,
    boot_dir,
    meson.project_source_root(),
  ],
)

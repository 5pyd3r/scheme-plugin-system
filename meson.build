project('scheme-plugin-system', 'c')

fs = import('fs')

# Map meson cpu_family to Chez Scheme machine type.
# 't' prefix = threaded (default), without = non-threaded.
cpu_map = {
  'aarch64': ['tarm64le', 'arm64le'],
  'x86_64':  ['a6le', 'ta6le'],
  'x86':     ['i3le', 'ti3le'],
  'arm':     ['arm32le', 'tarm32le'],
}
arch_dir = ''
foreach d : cpu_map.get(host_machine.cpu_family(), [])
  if fs.exists('chezscheme' / d / 'boot' / d / 'libkernel.a')
    arch_dir = d
    break
  endif
endforeach
# If not yet built, use the first (threaded) default
if arch_dir == ''
  arch_dir = cpu_map.get(host_machine.cpu_family(), ['unknown'])[0]
endif

message('Chez Scheme architecture: ' + arch_dir)

chez_src  = meson.project_source_root() / 'chezscheme'
chez_cfg  = chez_src / 'Makefile'
chez_boot_dir = chez_src / 'boot' / arch_dir
chez_lib_dir  = chez_src / arch_dir / 'boot' / arch_dir
chez_zlib_dir = chez_src / arch_dir / 'zlib'
chez_lz4_dir  = chez_src / arch_dir / 'lz4' / 'lib'

libkernel = chez_lib_dir / 'libkernel.a'
libz      = chez_zlib_dir / 'libz.a'
liblz4    = chez_lz4_dir / 'liblz4.a'

# ---- Cascading build: Chez Scheme ----
# custom_target runs configure + make if chezscheme hasn't been built yet.
# Outputs are the static libraries we link against.
# Meson tracks dependencies: if libkernel.a is missing or outdated,
# the build step runs automatically before linking.

chez_build = custom_target('chezscheme-build',
  output: ['libkernel.a', 'libz.a', 'liblz4.a'],
  command: [
    'sh', '-c',
    # Save the absolute build dir before cd
    'BUILD_DIR="$(pwd)" && ' +
    'echo "[meson] Building Chez Scheme..." && ' +
    'cd "@SOURCE_ROOT@/chezscheme" && ' +
    'CFLAGS="-fPIC -O2 -D_REENTRANT -pthread" ' +
    './configure --disable-x11 --disable-curses && ' +
    'make -j$(nproc) && ' +
    'echo "[meson] Chez Scheme build complete" && ' +
    # Copy with absolute paths: source relative to chezscheme, dest to build dir
    'echo "[meson] Copying libs to $BUILD_DIR..." && ' +
    'cp "' + arch_dir + '/boot/' + arch_dir + '/libkernel.a" "$BUILD_DIR/@OUTPUT0@" && ' +
    'cp "' + arch_dir + '/zlib/libz.a" "$BUILD_DIR/@OUTPUT1@" && ' +
    'cp "' + arch_dir + '/lz4/lib/liblz4.a" "$BUILD_DIR/@OUTPUT2@" && ' +
    'echo "[meson] Done copying"',
  ],
  build_by_default: true,
)

# Extract output paths as strings for link_args
chez_kernel_path = meson.project_build_root() / 'libkernel.a'
chez_zlib_path   = meson.project_build_root() / 'libz.a'
chez_lz4_path    = meson.project_build_root() / 'liblz4.a'

# Include directories (source tree, point to chezscheme submodule)
chez_includes = [
  include_directories('chezscheme' / 'c'),
  include_directories('chezscheme' / 'boot' / arch_dir),
]

# Link args: use the build-dir copies of chez libs, plus system libs
chez_link_args = [
  chez_kernel_path,
  chez_zlib_path,
  chez_lz4_path,
  '-lm', '-ldl', '-lpthread',
]

# ---- Shared Libraries ----

# Runtime shared library: petite.boot only (interpreter, no compiler)
libschemeplugin = shared_library('schemeplugin',
  'src/plugin.c',
  'src/chez_adaptor.c',
  include_directories: chez_includes,
  link_args: chez_link_args,
  link_depends: [chez_build],
  c_args: ['-DPLUGIN_SYSTEM'],
  install: true,
)

# Dev shared library: petite.boot + scheme.boot (interpreter + compiler)
libschemeplugin_dev = shared_library('schemeplugin-dev',
  'src/plugin.c',
  'src/chez_adaptor.c',
  include_directories: chez_includes,
  link_args: chez_link_args,
  link_depends: [chez_build],
  c_args: ['-DPLUGIN_SYSTEM', '-DPLUGIN_DEV'],
  install: true,
)

# ---- Demo Executable ----

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

# ---- Packaging ----

pkg_dir_release = meson.project_build_root() / 'release'
boot_dir_src    = chez_src / 'boot' / arch_dir
src_dir         = meson.project_source_root() / 'src'

run_target('package',
  command: [
    'sh', '-c',
    'rm -rf "$0/runtime" "$0/dev" && ' +
    # Runtime package (petite.boot only)
    'mkdir -p "$0/runtime/boot" && ' +
    'cp "$1/libschemeplugin.so" "$0/runtime/" && ' +
    'cp "$2/plugin.h" "$0/runtime/" && ' +
    'cp "$3/petite.boot" "$0/runtime/boot/" && ' +
    'cp "$4/README.md" "$0/runtime/" && ' +
    # Dev package (petite.boot + scheme.boot)
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
    boot_dir_src,
    meson.project_source_root(),
  ],
)

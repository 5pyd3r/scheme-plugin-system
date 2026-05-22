project('scheme-plugin-system', 'c')

# Chez Scheme submodule (pre-built via ./configure --disable-x11 --disable-curses && make)
chez_dir = meson.project_source_root() / 'chezscheme'

# Architecture-specific paths (auto-detected; tarm64le on ARM64, a6le on x86_64, etc.)
# Chez Scheme generates boot/<arch>/ and <arch>/boot/<arch>/
# We find the arch by listing directories containing libkernel.a
arch_dir = ''
foreach d : ['tarm64le', 'a6le', 'i3le', 'arm32le', 'ta6le', 'ti3le']
  lib = chez_dir / d / 'boot' / d / 'libkernel.a'
  if fs.exists(lib)
    arch_dir = d
  endif
endforeach

if arch_dir == ''
  error('Could not find Chez Scheme build output. Run: cd chezscheme && ./configure --disable-x11 --disable-curses && make')
endif

message('Chez Scheme architecture: ' + arch_dir)

chez_boot_dir = chez_dir / arch_dir / 'boot' / arch_dir
chez_zlib_dir = chez_dir / arch_dir / 'zlib'
chez_lz4_dir  = chez_dir / arch_dir / 'lz4' / 'lib'

# Also include the generated scheme.h from boot/<arch>/
chez_header_dir = chez_dir / 'boot' / arch_dir
chez_c_dir      = chez_dir / 'c'

# Chez Scheme static libraries (pre-built)
chez_libkernel = chez_boot_dir / 'libkernel.a'
chez_zlib      = chez_zlib_dir / 'libz.a'
chez_lz4       = chez_lz4_dir / 'liblz4.a'

executable('scheme-plugin-host',
  'src/main.c',
  'src/plugin.c',
  'src/chez_adaptor.c',
  include_directories: [
    include_directories(chez_c_dir),
    include_directories(chez_header_dir),
  ],
  link_args: [
    chez_libkernel,
    chez_zlib,
    chez_lz4,
    '-lm', '-ldl', '-lpthread',
  ],
  c_args: ['-DPLUGIN_SYSTEM'],
  install: true,
)

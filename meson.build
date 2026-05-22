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

# Chez Scheme as an external dependency
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

executable('scheme-plugin-host',
  'src/main.c',
  'src/plugin.c',
  'src/chez_adaptor.c',
  dependencies: [chez_dep],
  c_args: ['-DPLUGIN_SYSTEM'],
  install: true,
)

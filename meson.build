project('crisscross', 'cpp')

sources = [
  'source/compare.cpp',
  'source/console.cpp',
  'source/core_io_reader.cpp',
  'source/core_io_writer.cpp',
  'source/debug.cpp',
  'source/error.cpp',
  'source/filesystem.cpp',
  'source/stopwatch.cpp',
  'source/string_utils.cpp',
  'source/system.cpp',
  'source/universal_include.cpp',
  'source/version.cpp',
]
incdirs = include_directories('source', '.')

perl = find_program(['perl'])
gen_buildnum_h = custom_target('build_number.h',
                               input: [],
                               output: ['build_number.h'],
                               command: [perl, join_paths(meson.current_source_dir(), 'tools', 'gen_buildnum_h.pl'), '@OUTPUT@', 'CC_LIB', 'cc'])

lib = static_library(
  'crisscross',
  gen_buildnum_h,
  sources,
  include_directories: incdirs
)

crisscross_dep = declare_dependency(
  include_directories: incdirs,
  link_with: lib)

# vim: set ts=4 sts=4 sw=4 et:

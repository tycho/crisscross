project('crisscross', 'cpp')

sources = [
  'source/compare.cpp',
  'source/console.cpp',
  'source/core_io_reader.cpp',
  'source/core_io_writer.cpp',
  'source/debug.cpp',
  'source/error.cpp',
  'source/filereader.cpp',
  'source/filesystem.cpp',
  'source/filewriter.cpp',
  'source/md4.cpp',
  'source/md5.cpp',
  'source/mutex.cpp',
  'source/sha1.cpp',
  'source/sha256.cpp',
  'source/sha512.cpp',
  'source/stopwatch.cpp',
  'source/string_utils.cpp',
  'source/system.cpp',
  'source/tiger.cpp',
  'source/universal_include.cpp',
  'source/version.cpp',
  'source/zombiehunter.cpp',
]
incdirs = include_directories('source')

lib = static_library(
  'crisscross',
  sources,
  include_directories: incdirs
)

crisscross_dep = declare_dependency(
  include_directories: incdirs,
  link_with: lib)

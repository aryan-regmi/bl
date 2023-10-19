project('bl', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

# Includes
# =============================================
public_headers = include_directories('include')

# Library
# =============================================
sources = files([])
subdir('src')
bl_lib = library(
  'bl',
  sources,
  include_directories: [public_headers]
)

# Tests
# =============================================
subdir('tests')

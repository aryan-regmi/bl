string_tests = executable(
  'string_tests',
  'string_tests.cpp',
  include_directories: [public_headers],
  link_with: bl_lib,
)
test('String Tests', string_tests)

dyn_array_tests = executable(
  'dynamic_array_tests',
  'dynamic_array_tests.cpp',
  include_directories: [public_headers],
  link_with: bl_lib,
)
test('Dynamic Array Tests', dyn_array_tests)

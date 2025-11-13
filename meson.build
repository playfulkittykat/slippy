project(
    'slippy',
    'c',
    version: '0.1',  # Also update library.json!
    default_options: ['c_std=c99', 'warning_level=3'],
    license: 'AGPL-3.0-or-later',
)

incdir = include_directories('include')

lib = library(
    'slippy',
    'src/slippy.c',
    install: true,
    include_directories: incdir,
)

executable(
    'slippy-encode',
    ['examples/encode.c'],
    include_directories: incdir,
    link_with: [lib],
    install: false,
)

executable(
    'slippy-decode',
    ['examples/decode.c'],
    include_directories: incdir,
    link_with: [lib],
    install: false,
)

criterion = dependency('criterion', required: false)

if criterion.found()
    test_reader = executable(
        'test_reader',
        ['tests/test_reader.c'],
        include_directories: incdir,
        dependencies: [criterion],
        link_with: [lib],
        install: false,
    )
    test('test_reader', test_reader)

    test_writer = executable(
        'test_writer',
        ['tests/test_writer.c'],
        include_directories: incdir,
        dependencies: [criterion],
        link_with: [lib],
        install: false,
    )
    test('test_writer', test_writer)
endif

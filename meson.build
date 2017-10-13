project('irstore', 'c', 'cpp')

add_global_arguments('-std=c++1y', language : 'cpp')

c = meson.get_compiler('c')

irlib = static_library('irstore', 
    'file.cpp',
    'irerror.cpp',
    'store.cpp',
    'lmdbfile.cpp'
)


executable('storetest', 
    'storetest.c', 
        link_with: irlib) 

executable('bitvectest', 
    'bitvectest.cpp', 
        link_with: irlib)

executable('lmdbtest', 
    'lmdbtest.cpp', 
        link_args: '/usr/local/lib/liblmdb.a',
        link_with: irlib)
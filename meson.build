project('irstore', 'c', 'cpp')

add_global_arguments('-std=c++1y', language: 'cpp')

c = meson.get_compiler('c')

thread_dep = dependency('threads')
log_with_trace = declare_dependency(compile_args: '-DSPDLOG_TRACE_ON')

#--- irstore lib ----
irlib_files = [ 'file.cpp', 'irerror.cpp', 'store.cpp', 'lmdbfile.cpp']
irlib = static_library('irstore',  irlib_files)
irlib_debug = static_library('irstore_debug', irlib_files, dependencies: log_with_trace)
#--- irstore lib ----

executable('storetest', 
    'storetest.c', 
        link_with: irlib) 

executable('bitvectest', 
    'bitvectest.cpp', 
        link_with: irlib)

executable('lmdbtest', 
    'lmdbtest.cpp', 
        link_args: '/usr/local/lib/liblmdb.a',
        link_with: irlib_debug,
        dependencies: [log_with_trace, thread_dep])
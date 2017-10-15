project('irstore', 'c', 'cpp')

add_global_arguments('-std=c++1y', language: 'cpp')

c = meson.get_compiler('c')

##########################################
# Global include dirs
##########################################
irstore_headers = declare_dependency(compile_args: '-I' + meson.source_root())

##########################################
# Global libraries 
##########################################
thread_dep = dependency('threads')
log_with_trace = declare_dependency(compile_args: '-DSPDLOG_TRACE_ON')
cpp_coverage = declare_dependency(
    compile_args:'--coverage',
    link_args: '-lgcov')

doctest_dep = declare_dependency(compile_args: '-I/usr/local/include/doctest')
lmdb_dep = declare_dependency(link_args: '/usr/local/lib/liblmdb.a')

#--- irstore lib ----
irlib_files = [ 'file.cpp', 'irerror.cpp', 'store.cpp', 'lmdbfile.cpp']
irlib = static_library('irstore',  irlib_files)
irlib_debug = static_library('irstore_debug', irlib_files, 
    dependencies: [log_with_trace, cpp_coverage])
#--- irstore lib ----

executable('storetest', 
    'storetest.c', 
        link_with: irlib) 



subdir('tests')


irerror_test = executable('irerror_test', 
    'irerror_test.cpp', 
        link_with: irlib_debug,
        dependencies: [doctest_dep, irstore_headers]) 
        
lmdb_test = executable('lmdbtest', 
    'lmdbtest.cpp', 
        link_with: irlib_debug,
        dependencies: [irstore_headers, lmdb_dep, doctest_dep, log_with_trace, cpp_coverage, thread_dep])

sparsefile_test = executable('sparsefiletest', 
    'sparsefiletest.cpp', 
        link_with: irlib_debug,
        dependencies: [irstore_headers, lmdb_dep, doctest_dep, log_with_trace, cpp_coverage, thread_dep])

patch_test = executable('patchtest', 
    'sparsefiletest.cpp', 
        link_with: irlib_debug,
        dependencies: [irstore_headers, doctest_dep, log_with_trace, cpp_coverage])
#bitvec_test = executable('bitvectest', 
#    'bitvectest.cpp', 
#        link_with: irlib_debug,
#        dependencies: [irstore_headers, doctest_dep, cpp_coverage])

test('irerror', irerror_test)
test('lmdb', lmdb_test)
test('sparsefile', sparsefile_test)
test('patch', patch_test)
#test('bitvec', bitvec_test)
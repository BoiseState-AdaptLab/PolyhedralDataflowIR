cmake_minimum_required(VERSION 3.10)
project(artifact C CXX)
include_directories(include include/common include/mfd include/chill lib/oski lib/oski/include)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fPIC -DPIC -Xpreprocessor -fopenmp -ftree-vectorize -funroll-all-loops -O3")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -fPIC -DPIC -Xpreprocessor -fopenmp -ftree-vectorize -funroll-all-loops -O3")

add_executable(csr_bsr_insp src/pdfl/csr_bsr_insp.c)
target_link_libraries(csr_bsr_insp m omp) # blas lapack lapacke splatt) # oski)

add_executable(coo_csf_insp src/pdfl/coo_csf_insp.c)
target_link_libraries(coo_csf_insp m omp) # blas lapack lapacke splatt) # oski)

add_executable(chill_bsr src/chill/matrix_io.cc src/chill/smv.cc src/common/Benchmark.cc src/common/Configuration.cc
                          src/common/Measurements.cc src/chill/rose_main.cc src/chill/bmark_main.cc)
target_link_libraries(chill_bsr m omp) # blas lapack lapacke splatt) # oski)

add_executable(oski_bsr src/oski/oski_main.c lib/oski/src/init.c lib/oski/src/lt/modsyms.c)
target_compile_definitions(oski_bsr PRIVATE OSKI_LIB_PATH_DEFAULT=\"lib/oski\" OSKI_LUA_ROOT_DIR=\"lib/oski\"
                                    OSKI_CC=\"${CC}\" OSKI_CFLAGS=\"${CMAKE_C_FLAGS}\" OSKI_LDFLAGS=\"CMAKE_STATIC_LINKER_FLAGS\"
                                    OSKI_F77=\"f77\" OSKI_FFLAGS=\"\" OSKI_SUPPORT_LDFLAGS=\"$(PAPI_LIBS)\"
                                    OSKI_BENCH_DIR_DEFAULT=\"lib/oski/bench\")



add_executable(mfd_series src/mfd/mfd_series.cc src/common/Benchmark.cc src/common/Configuration.cc
                          src/common/Measurements.cc src/mfd/MiniFluxBenchmark-series.cc)
set_target_properties(mfd_series PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(mfd_series m omp)

add_executable(mfd_fuse_all src/mfd/mfd_fuse_all.cc src/common/Benchmark.cc src/common/Configuration.cc
                            src/common/Measurements.cc src/mfd/MiniFluxBenchmark-fuseAll.cc)
set_target_properties(mfd_fuse_all PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(mfd_fuse_all m omp)

add_executable(mfd_fuse_within src/mfd/mfd_fuse_within.cc src/common/Benchmark.cc src/common/Configuration.cc
                               src/common/Measurements.cc src/mfd/MiniFluxBenchmark-fuseWithin.cc)
set_target_properties(mfd_fuse_within PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(mfd_fuse_within m omp)

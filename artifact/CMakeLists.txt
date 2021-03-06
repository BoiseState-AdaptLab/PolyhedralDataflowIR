cmake_minimum_required(VERSION 3.02)
project(artifact C CXX)
include_directories(include include/common include/mfd include/chill lib/oski lib/oski/include lib/splatt/include)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fPIC -DPIC -Xpreprocessor -fopenmp -ftree-vectorize -funroll-all-loops -O3")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -g -fPIC -DPIC -Xpreprocessor -fopenmp -ftree-vectorize -funroll-all-loops -O3")

set(OSKI_BUILD "true")
if (OSKI_BUILD)
    execute_process(COMMAND oski_build.sh WORKING_DIRECTORY ./lib)
endif()

# Sparse Matrix Case Study
add_executable(csr_bsr_insp src/pdfl/csr_bsr_insp.c)
target_link_libraries(csr_bsr_insp m omp)

add_executable(chill_bsr src/chill/matrix_io.cc src/chill/smv.cc src/common/Benchmark.cc src/common/Configuration.cc
        src/common/Measurements.cc src/chill/rose_main.cc src/chill/bmark_main.cc)
target_link_libraries(chill_bsr m omp)

link_directories(lib/oski/lib/oski)
add_executable(oski_bsr src/oski/oski_main.c)
target_link_libraries(oski_bsr m omp oski oskilt oski_Tid oski_mat_CSR_Tid oski_mat_CSC_Tid oski_mat_BCSR_Tid
        oski_mat_MBCSR_Tid oski_mat_GCSR_Tid oski_mat_CB_Tid oski_mat_VBR_Tid oski_mat_DENSE_Tid
        oski_heur_regprof_Tid oski_heur_symmrb_Tid oski_heur_mregblock_Tid)

set(SPLATT_BUILD "true")
if (SPLATT_BUILD)
    execute_process(COMMAND splatt_build.sh WORKING_DIRECTORY ./lib)
endif()

# Sparse Tensor Case Study
link_directories(lib/splatt/lib lib/splatt/build/Linux-x86_64/blis/lib)
add_executable(coo_csf_insp src/pdfl/coo_csf_insp.c)
target_link_libraries(coo_csf_insp m omp) # blis splatt)
#target_compile_definitions(coo_csf_insp PRIVATE SPLATT_ENABLED=1)

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

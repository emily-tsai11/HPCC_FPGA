# This file contains the default configuration for the Nallatech 520N board
# for the use with single precision floating point values.
# To use this configuration file, call cmake with the parameter
#
#     cmake [...] -DHPCC_FPGA_CONFIG="path to this file"
#

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
set(USE_MPI No CACHE BOOL "" FORCE)
set(USE_SVM Yes CACHE BOOL "" FORCE)
set(USE_HBM No CACHE BOOL "" FORCE)
set(FPGA_BOARD_NAME "pac_s10_usm" CACHE STRING "" FORCE)
set(AOC_FLAGS "-fpc -fp-relaxed -no-interleaving=default" CACHE STRING "" FORCE)

# GEMM specific options
set(DEFAULT_MATRIX_SIZE 8 CACHE STRING "Default size of the used matrices" FORCE)
set(BLOCK_SIZE 256 CACHE STRING "Block size used in the FPGA kernel" FORCE)
set(GEMM_BLOCK 8 CACHE STRING "Block size used in the FPGA kernel for the cannon algorithm" FORCE)
set(DATA_TYPE float CACHE STRING "Data type used for kernel and host code" FORCE)
set(GLOBAL_MEM_UNROLL 8 CACHE STRING "Unrolling factor used to stream data" FORCE)
set(NUM_REPLICATIONS 5 CACHE STRING "Number of times the kernels will be replicated" FORCE)
set(INTEL_CODE_GENERATION_SETTINGS ${CMAKE_SOURCE_DIR}/settings/settings.gen.intel.gemm_base.svm.py CACHE FILEPATH "" FORCE)

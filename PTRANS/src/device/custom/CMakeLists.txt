include(${CMAKE_SOURCE_DIR}/../cmake/customKernelTargets.cmake)

foreach (target ${custom_kernel_targets})
    if (INTELFPGAOPENCL_FOUND)
        add_test(NAME test_${target}_emulation_intel COMMAND Transpose_intel -f ${target}_emulate.aocx -n 1 -m 1 WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
    endif()

    if (Vitis_FOUND)
        add_test(NAME test_${target}_emulation_xilinx COMMAND Transpose_xilinx -f ${target}_emulate.xclbin -n 1 -m 1 WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
    endif()
endforeach(target)
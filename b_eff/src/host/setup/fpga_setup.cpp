//
// Created by Marius Meyer on 04.12.19.
//

#include "fpga_setup.hpp"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>

/* External libraries */
#include "parameters.h"
#include "mpi.h"

namespace fpga_setup {

    /**
Converts the reveived OpenCL error to a string

@param err The OpenCL error code

@return The string representation of the OpenCL error code
*/
    std::string
    getCLErrorString(cl_int const err) {
        switch (err) {
            CL_ERR_TO_STR(CL_DEVICE_NOT_FOUND);
            CL_ERR_TO_STR(CL_DEVICE_NOT_AVAILABLE);
            CL_ERR_TO_STR(CL_COMPILER_NOT_AVAILABLE);
            CL_ERR_TO_STR(CL_MEM_OBJECT_ALLOCATION_FAILURE);
            CL_ERR_TO_STR(CL_OUT_OF_RESOURCES);
            CL_ERR_TO_STR(CL_OUT_OF_HOST_MEMORY);
            CL_ERR_TO_STR(CL_PROFILING_INFO_NOT_AVAILABLE);
            CL_ERR_TO_STR(CL_MEM_COPY_OVERLAP);
            CL_ERR_TO_STR(CL_IMAGE_FORMAT_MISMATCH);
            CL_ERR_TO_STR(CL_IMAGE_FORMAT_NOT_SUPPORTED);
            CL_ERR_TO_STR(CL_BUILD_PROGRAM_FAILURE);
            CL_ERR_TO_STR(CL_MAP_FAILURE);
            CL_ERR_TO_STR(CL_MISALIGNED_SUB_BUFFER_OFFSET);
            CL_ERR_TO_STR(CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST);
            CL_ERR_TO_STR(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
            CL_ERR_TO_STR(CL_INVALID_VALUE);
            CL_ERR_TO_STR(CL_INVALID_DEVICE_TYPE);
            CL_ERR_TO_STR(CL_INVALID_PLATFORM);
            CL_ERR_TO_STR(CL_INVALID_DEVICE);
            CL_ERR_TO_STR(CL_INVALID_CONTEXT);
            CL_ERR_TO_STR(CL_INVALID_QUEUE_PROPERTIES);
            CL_ERR_TO_STR(CL_INVALID_COMMAND_QUEUE);
            CL_ERR_TO_STR(CL_INVALID_HOST_PTR);
            CL_ERR_TO_STR(CL_INVALID_MEM_OBJECT);
            CL_ERR_TO_STR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
            CL_ERR_TO_STR(CL_INVALID_IMAGE_SIZE);
            CL_ERR_TO_STR(CL_INVALID_SAMPLER);
            CL_ERR_TO_STR(CL_INVALID_BINARY);
            CL_ERR_TO_STR(CL_INVALID_BUILD_OPTIONS);
            CL_ERR_TO_STR(CL_INVALID_PROGRAM);
            CL_ERR_TO_STR(CL_INVALID_PROGRAM_EXECUTABLE);
            CL_ERR_TO_STR(CL_INVALID_KERNEL_NAME);
            CL_ERR_TO_STR(CL_INVALID_KERNEL_DEFINITION);
            CL_ERR_TO_STR(CL_INVALID_KERNEL);
            CL_ERR_TO_STR(CL_INVALID_ARG_INDEX);
            CL_ERR_TO_STR(CL_INVALID_ARG_VALUE);
            CL_ERR_TO_STR(CL_INVALID_ARG_SIZE);
            CL_ERR_TO_STR(CL_INVALID_KERNEL_ARGS);
            CL_ERR_TO_STR(CL_INVALID_WORK_DIMENSION);
            CL_ERR_TO_STR(CL_INVALID_WORK_GROUP_SIZE);
            CL_ERR_TO_STR(CL_INVALID_WORK_ITEM_SIZE);
            CL_ERR_TO_STR(CL_INVALID_GLOBAL_OFFSET);
            CL_ERR_TO_STR(CL_INVALID_EVENT_WAIT_LIST);
            CL_ERR_TO_STR(CL_INVALID_EVENT);
            CL_ERR_TO_STR(CL_INVALID_OPERATION);
            CL_ERR_TO_STR(CL_INVALID_GL_OBJECT);
            CL_ERR_TO_STR(CL_INVALID_BUFFER_SIZE);
            CL_ERR_TO_STR(CL_INVALID_MIP_LEVEL);
            CL_ERR_TO_STR(CL_INVALID_GLOBAL_WORK_SIZE);
            CL_ERR_TO_STR(CL_COMPILE_PROGRAM_FAILURE);
            CL_ERR_TO_STR(CL_LINKER_NOT_AVAILABLE);
            CL_ERR_TO_STR(CL_LINK_PROGRAM_FAILURE);
            CL_ERR_TO_STR(CL_DEVICE_PARTITION_FAILED);
            CL_ERR_TO_STR(CL_INVALID_PROPERTY);
            CL_ERR_TO_STR(CL_INVALID_IMAGE_DESCRIPTOR);
            CL_ERR_TO_STR(CL_INVALID_COMPILER_OPTIONS);
            CL_ERR_TO_STR(CL_INVALID_LINKER_OPTIONS);
            CL_ERR_TO_STR(CL_INVALID_DEVICE_PARTITION_COUNT);

            default:
                return "UNKNOWN ERROR CODE";
        }
    }

/**
Check the OpenCL return code for errors.
If an error is detected, it will be printed and the programm execution is
stopped.

@param err The OpenCL error code
*/
    void
    handleClReturnCode(cl_int const err, std::string const file,
                       int const line) {
        if (err != CL_SUCCESS) {
            std::string err_string = getCLErrorString(err);
            std::cerr << "ERROR in OpenCL library detected! Aborting."
                      << std::endl << file << ":" << line << ": " << err_string
                      << std::endl;
            exit(err);
        }
    }

/**
Sets up the given FPGA with the kernel in the provided file.

@param context The context used for the program
@param program The devices used for the program
@param usedKernelFile The path to the kernel file
@return The program that is used to create the benchmark kernels
*/
    cl::Program
    fpgaSetup(const cl::Context *context, std::vector<cl::Device> deviceList,
              const std::string *usedKernelFile) {
        int err;
        int world_rank;
        int world_size;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        if (world_rank == 0) {
            std::cout << HLINE;
            std::cout << "FPGA Setup:" << usedKernelFile->c_str() << std::endl;
        }

        // Open file stream if possible
        std::ifstream aocxStream(usedKernelFile->c_str(), std::ifstream::binary);
        if (!aocxStream.is_open()) {
            std::cerr << "Not possible to open from given file!" << std::endl;
        }

        // Read in file contents and create program from binaries
        std::string prog(std::istreambuf_iterator<char>(aocxStream),
                         (std::istreambuf_iterator<char>()));
        aocxStream.seekg(0, aocxStream.end);
        unsigned file_size = aocxStream.tellg();
        aocxStream.seekg(0, aocxStream.beg);
        char *buf = new char[file_size];
        aocxStream.read(buf, file_size);

        cl::Program::Binaries mybinaries;
        mybinaries.push_back({buf, file_size});

        // Create the Program from the AOCX file.
        cl::Program program(*context, deviceList, mybinaries, NULL, &err);
        ASSERT_CL(err);
        if (world_rank == 0) {
            std::cout << "Prepared FPGA successfully for global Execution!" <<
                      std::endl;
            std::cout << HLINE;
        }
        return program;
    }

/**
Sets up the C++ environment by configuring std::cout and checking the clock
granularity using bm_helper::checktick()
*/
    void
    setupEnvironmentAndClocks() {
        std::cout << std::setprecision(5) << std::scientific;

        std::cout << HLINE;
        std::cout << "General setup:" << std::endl;

        // Check clock granularity and output result
        std::cout << "C++ high resolution clock is used." << std::endl;
        std::cout << "The clock precision seems to be "
                  << static_cast<double>
                     (std::chrono::high_resolution_clock::period::num) /
                     std::chrono::high_resolution_clock::period::den * 10e9
                  << "ns" << std::endl;

        std::cout << HLINE;
    }


/**
Searches an selects an FPGA device using the CL library functions.
If multiple platforms or devices are given, the user will be prompted to
choose a device.

@param defaultPlatform The index of the platform that has to be used. If a
                        value < 0 is given, the platform can be chosen
                        interactively
@param defaultDevice The index of the device that has to be used. If a
                        value < 0 is given, the device can be chosen
                        interactively

@return A list containing a single selected device
*/
    std::vector<cl::Device>
    selectFPGADevice(int defaultPlatform, int defaultDevice) {
        // Integer used to store return codes of OpenCL library calls
        int err;

        int world_rank;
        int world_size;
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        std::vector<cl::Platform> platformList;
        err = cl::Platform::get(&platformList);
        ASSERT_CL(err);

        // Choose the target platform
        int chosenPlatformId = 0;
        if (defaultPlatform >= 0) {
            if (defaultPlatform < platformList.size()) {
                chosenPlatformId = defaultPlatform;
            } else {
                std::cerr << "Default platform " << defaultPlatform
                          << " can not be used. Available platforms: "
                          << platformList.size() << std::endl;
                exit(1);
            }
        } else if (platformList.size() > 1 && world_size == 1) {
            std::cout <<
                      "Multiple platforms have been found. Select the platform by"\
            " typing a number:" << std::endl;
            for (int platformId = 0;
                 platformId < platformList.size(); platformId++) {
                std::cout << platformId << ") " <<
                          platformList[platformId].getInfo<CL_PLATFORM_NAME>() <<
                          std::endl;
            }
            std::cout << "Enter platform id [0-" << platformList.size() - 1
                      << "]:";
            std::cin >> chosenPlatformId;
        }
        cl::Platform platform = platformList[chosenPlatformId];
        if (world_rank == 0) {
            std::cout << "Selected Platform: "
                      << platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
        }
        std::vector<cl::Device> deviceList;
        err = platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &deviceList);
        ASSERT_CL(err);

        // Choose taget device
        int chosenDeviceId = 0;
        if (defaultDevice >= 0) {
            if (defaultDevice < deviceList.size()) {
                chosenDeviceId = defaultDevice;
            } else {
                std::cerr << "Default platform " << defaultDevice
                          << " can not be used. Available platforms: "
                          << deviceList.size() << std::endl;
                exit(1);
            }
        } else if (deviceList.size() > 1) {
            if (world_size == 1) {
                    std::cout <<
                              "Multiple devices have been found. Select the platform by"\
                            " typing a number:" << std::endl;

                for (int deviceId = 0;
                     deviceId < deviceList.size(); deviceId++) {
                    std::cout << deviceId << ") " <<
                              deviceList[deviceId].getInfo<CL_DEVICE_NAME>() <<
                              std::endl;
                }
                std::cout << "Enter device id [0-" << deviceList.size() - 1 << "]:";
                std::cin >> chosenDeviceId;
            } else {
                chosenDeviceId = static_cast<int>(world_rank % deviceList.size());
            }
        }
        std::vector<cl::Device> chosenDeviceList;
        chosenDeviceList.push_back(deviceList[chosenDeviceId]);

        if (world_rank == 0) {
            // Give selection summary
            std::cout << HLINE;
            std::cout << "Selection summary:" << std::endl;
            std::cout << "Platform Name: " <<
                      platform.getInfo<CL_PLATFORM_NAME>() << std::endl;
            std::cout << "Device Name:   " <<
                      chosenDeviceList[0].getInfo<CL_DEVICE_NAME>() << std::endl;
            std::cout << HLINE;
        }

        return chosenDeviceList;
    }

}  // namespace fpga_setup
/*
Copyright (c) 2019 Marius Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* Related header files */
#include "execution.h"

/* C++ standard library headers */
#include <memory>
#include <vector>
#include <chrono>

/* External library headers */
#include "CL/cl.hpp"
#include "CL/cl_ext_intelfpga.h"
#include "mpi.h"

/* Project's headers */

namespace bm_execution {

    /*
    Implementation for the single kernel.
     @copydoc bm_execution::calculate()
    */
    std::shared_ptr<ExecutionTimings>
    calculate(std::shared_ptr<ExecutionConfiguration> config, cl_uint messageSize, cl_uint looplength) {


        cl::Kernel sendKernel(config->program, SEND_KERNEL_NAME);

        sendKernel.setArg(0, messageSize);
        sendKernel.setArg(1, looplength);

        cl::Kernel recvKernel(config->program, RECV_KERNEL_NAME);

        recvKernel.setArg(0, messageSize);
        recvKernel.setArg(1, looplength);

        cl::CommandQueue sendQueue(config->context);
        cl::CommandQueue recvQueue(config->context);

        std::vector<double> calculationTimings;
        for (uint r =0; r < config->repetitions; r++) {
            MPI_Barrier(MPI_COMM_WORLD);
            auto startCalculation = std::chrono::high_resolution_clock::now();
            sendQueue.enqueueTask(sendKernel);
            recvQueue.enqueueTask(recvKernel);
            recvQueue.finish();
            sendQueue.finish();
            auto endCalculation = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> calculationTime =
                    std::chrono::duration_cast<std::chrono::duration<double>>
                            (endCalculation - startCalculation);
            calculationTimings.push_back(calculationTime.count());
        }

        std::shared_ptr<ExecutionTimings> result(new ExecutionTimings{
                looplength,
                messageSize,
                calculationTimings
        });
        return result;
    }

}  // namespace bm_execution
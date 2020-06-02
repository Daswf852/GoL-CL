#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>

#include <CL/cl.hpp>

class CLManager {
    public:
        CLManager();
        ~CLManager();

        void CLSetup();

        cl::Kernel LoadCLKernelFromPath(std::string file);
        cl::Kernel LoadCLKernelFromString(std::string kernelCode);

        cl::Context &GetCLContext();
        cl::Device &GetCLDevice();
        unsigned int GetMaxGroups();
        bool GetIsCLSetUp();
    
    private:
        bool isCLSetUp = false;
        cl::Context clContext;
        cl::Device clDevice;
        unsigned int maxGroups = 0;

        static std::string dummyKernel;
};
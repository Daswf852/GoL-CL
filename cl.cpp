#include "cl.hpp"

CLManager::CLManager() {
    CLSetup();
}

CLManager::~CLManager() {

}

void CLManager::CLSetup() {
    if (isCLSetUp) return;

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.size() == 0) {
        std::cout<<"OpenCL could not be set up. No OpenCL platforms are found"<<std::endl;
        isCLSetUp = false;
        return;
    }

    cl::Platform platform = platforms.at(0);

    std::cout<<"CL platform: "<<platform.getInfo<CL_PLATFORM_NAME>()<<std::endl;

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if (devices.size() == 0) {
        std::cout<<"OpenCL could not be set up. No OpenCL devices are found"<<std::endl;
        isCLSetUp = false;
        return;
    }

    clDevice = devices.at(0);

    std::cout<<"CL device: "<<clDevice.getInfo<CL_DEVICE_NAME>()<<std::endl;

    maxGroups = clDevice.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();

    clContext = cl::Context({clDevice});

    isCLSetUp = true;
}

cl::Kernel CLManager::LoadCLKernelFromPath(std::string file) {
    if (!isCLSetUp) CLSetup();
    assert(isCLSetUp);

    std::string kernelSource;
    std::ifstream ifs(file);

    if (!ifs.is_open() || file == "default") {
        kernelSource = dummyKernel;
    } else{
        std::ostringstream oss;
        oss<<ifs.rdbuf();
        kernelSource = oss.str();
    }

    if (ifs.is_open()) ifs.close();

    return LoadCLKernelFromString(kernelSource);
}

cl::Kernel CLManager::LoadCLKernelFromString(std::string kernelCode) {
    cl::Program::Sources kernelSources;
    kernelSources.push_back({kernelCode.c_str(), kernelCode.size()});

    cl::Program program(clContext, kernelSources);
    if (program.build({clDevice}, "") != CL_SUCCESS) {
        std::cout<<program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(clDevice)<<std::endl;
        if (program.build({clDevice}) != CL_SUCCESS) {
            return LoadCLKernelFromString(dummyKernel);
        }
    }

    cl::Kernel programKernel(program, "kmain");
    return programKernel;
}

cl::Context &CLManager::GetCLContext() {
    return clContext;
}

cl::Device &CLManager::GetCLDevice() {
    return clDevice;
}

unsigned int CLManager::GetMaxGroups() {
    return maxGroups;
}

bool CLManager::GetIsCLSetUp() {
    return isCLSetUp;
}

std::string CLManager::dummyKernel = "void kernel kmain() {}";
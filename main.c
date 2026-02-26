#include <stdio.h>
#include <curses.h>

#include "/opt/cuda/include/nvml.h"

typedef struct {
    unsigned int code;
    const char* name;
} Arch;

Arch archs[] = {
    {NVML_DEVICE_ARCH_MAXWELL, "Maxwell"},
    {NVML_DEVICE_ARCH_PASCAL,  "Pascal"},
    {NVML_DEVICE_ARCH_VOLTA,   "Volta"},
    {NVML_DEVICE_ARCH_TURING,  "Turing"},
    {NVML_DEVICE_ARCH_AMPERE,  "Ampere"},
    {NVML_DEVICE_ARCH_ADA,     "Ada"},
    {NVML_DEVICE_ARCH_HOPPER,  "Hopper"},
    {NVML_DEVICE_ARCH_BLACKWELL, "Blackwell"},
    {NVML_DEVICE_ARCH_UNKNOWN, "Unknown"}
};

const char* getArchName(unsigned int code) {
    int n = sizeof(archs)/sizeof(archs[0]);
    for (int i = 0; i < n; i++) {
        if (archs[i].code == code) return archs[i].name;
    }
    return "Invalid";
}

void initUI(void) {
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
}

void drawUI(void) {
	
}

int main(void) {
	
	initUI();

	nvmlDevice_t dev;

	unsigned int pwr;
	unsigned int temp;
	unsigned int clockMhz;
	unsigned int fanSpeed;
	unsigned int boardId;
	unsigned int pcieGen;
	unsigned int maxGen;
	unsigned int pathNum;
	unsigned int arch;
	
	int cuda;

	char driver[100];
	char nvml[100];
	char vbios[100];
	char model[64];

	nvmlMemory_t memInfo;

	nvmlDeviceArchitecture_t archList;

	nvmlReturn_t initResult = nvmlInit_v2();
	nvmlReturn_t handleResult = nvmlDeviceGetHandleByIndex(0, &dev);

	nvmlReturn_t pwrResult =  nvmlDeviceGetPowerUsage(dev, &pwr);
	nvmlReturn_t tempResult = nvmlDeviceGetTemperature(dev, NVML_TEMPERATURE_GPU, &temp);
	nvmlReturn_t clockResult = nvmlDeviceGetClock(dev, NVML_CLOCK_SM, NVML_CLOCK_ID_CURRENT, &clockMhz);
	nvmlReturn_t fanResult = nvmlDeviceGetFanSpeed(dev, &fanSpeed);
	nvmlReturn_t boardResult = nvmlDeviceGetBoardId(dev, &boardId);
	nvmlReturn_t pcieGenResult = nvmlDeviceGetCurrPcieLinkGeneration(dev, &pcieGen);
	nvmlReturn_t maxGenResult = nvmlDeviceGetMaxPcieLinkGeneration(dev, &maxGen);
	nvmlReturn_t pathResult = nvmlDeviceGetMinorNumber(dev, &pathNum);

	nvmlReturn_t cudaResult = nvmlSystemGetCudaDriverVersion(&cuda);

	nvmlReturn_t driverResult = nvmlSystemGetDriverVersion(driver, NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);
	nvmlReturn_t nvmlResult = nvmlSystemGetNVMLVersion(nvml, NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE);
	nvmlReturn_t vbiosResult = nvmlDeviceGetVbiosVersion(dev, vbios, NVML_DEVICE_VBIOS_VERSION_BUFFER_SIZE);
	nvmlReturn_t modelResult = nvmlDeviceGetName(dev, model, NVML_DEVICE_NAME_BUFFER_SIZE);

	nvmlReturn_t memResult = nvmlDeviceGetMemoryInfo(dev, &memInfo);
	nvmlReturn_t archResult = nvmlDeviceGetArchitecture(dev, &arch);

	nvmlReturn_t setSpeed = nvmlDeviceSetFanSpeed_v2(dev, 0, 90); 
	
	if(initResult != NVML_SUCCESS) printf("Failed to init NVML.\n");
	if(handleResult != NVML_SUCCESS) printf("Failed to get GPU handle.\n");
	
	if(pwrResult != NVML_SUCCESS) {
    	printf("Failed to get power usage: %s\n", nvmlErrorString(pwrResult));
	} else {
    	printf("Power: %d W\n", pwr / 1000);
	} 
	
	if(setSpeed != NVML_SUCCESS) {
		printf("Failed to set fan speed : %s\n", nvmlErrorString(setSpeed));
	}

	double used = memInfo.used / 1e9;
	double total = memInfo.total / 1e9;
	
	nvmlDeviceSetFanSpeed_v2(dev, 1, 90);

	printf("Temp: %d°C\n", temp);
	printf("GPU Clock : %u Mhz\n", clockMhz);
	printf("GPU fan speed : %u %%\n", fanSpeed);
	printf("GPU Board ID: %u\n", boardId);
	printf("vBIOS version : %s\n", vbios);
	printf("GPU : %s\n", model);
	printf("PCIe Link generation : %u\n", pcieGen);
	printf("PCIe Max Link gen : %u\n", maxGen);
	printf("Memory : %.3f / %.3f GB\n", used, total);
	printf("Device file path: /dev/nvidia%u\n", pathNum);
	printf("GPU arch : %s\n", getArchName(arch));
	
	printf("CUDA Driver Version: %d\n", cuda / 1000);
	printf("NVIDIA Driver Version: %s\n", driver);
	printf("NVML Version: %s\n", nvml);

	nvmlShutdown();
}

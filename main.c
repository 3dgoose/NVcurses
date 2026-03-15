#include "/opt/cuda/include/nvml.h"

#include <unistd.h>
#include <ncurses.h>

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

int main(void) {
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);

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
	
	nvmlReturn_t maxGenResult = nvmlDeviceGetMaxPcieLinkGeneration(dev, &maxGen);
	nvmlReturn_t pathResult = nvmlDeviceGetMinorNumber(dev, &pathNum);

	nvmlReturn_t cudaResult = nvmlSystemGetCudaDriverVersion(&cuda);

	nvmlReturn_t driverResult = nvmlSystemGetDriverVersion(driver, NVML_SYSTEM_DRIVER_VERSION_BUFFER_SIZE);
	nvmlReturn_t nvmlResult = nvmlSystemGetNVMLVersion(nvml, NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE);
	nvmlReturn_t vbiosResult = nvmlDeviceGetVbiosVersion(dev, vbios, NVML_DEVICE_VBIOS_VERSION_BUFFER_SIZE);
	nvmlReturn_t modelResult = nvmlDeviceGetName(dev, model, NVML_DEVICE_NAME_BUFFER_SIZE);

	nvmlReturn_t memResult = nvmlDeviceGetMemoryInfo(dev, &memInfo);
	nvmlReturn_t archResult = nvmlDeviceGetArchitecture(dev, &arch);

	if(initResult != NVML_SUCCESS) printf("Failed to init NVML.\n");
	if(handleResult != NVML_SUCCESS) printf("Failed to get GPU handle.\n");
	
	if(pwrResult != NVML_SUCCESS) {
    	printf("Failed to get power usage: %s\n", nvmlErrorString(pwrResult));
	} else {
    	printf("Power: %d W\n", pwr / 1000);
	} 
	
	double used = memInfo.used / (double)(1ULL << 30);
	double total = memInfo.total / (double)(1ULL << 30);

	while (1) {

		int ch = getch();
		if (ch == 'q' || ch == 'Q')
			break;

		nvmlDeviceGetPowerUsage(dev, &pwr);
		nvmlDeviceGetTemperature(dev, NVML_TEMPERATURE_GPU, &temp);
		nvmlDeviceGetClock(dev, NVML_CLOCK_SM, NVML_CLOCK_ID_CURRENT, &clockMhz);
		nvmlReturn_t pcieGenResult = nvmlDeviceGetCurrPcieLinkGeneration(dev, &pcieGen);
		nvmlDeviceGetFanSpeed(dev, &fanSpeed);
		nvmlDeviceGetMemoryInfo(dev, &memInfo);	

		double used = memInfo.used / (double)(1ULL << 30);
		double total = memInfo.total / (double)(1ULL << 30);

		clear();

		printw("Temp: %d°C\n", temp);
		printw("GPU Clock : %u Mhz\n", clockMhz);
		printw("GPU fan speed : %u %%\n", fanSpeed);
		printw("GPU Board ID: %u\n", boardId);
		printw("vBIOS version : %s\n", vbios);
		printw("GPU : %s\n", model);
		printw("PCIe Link generation : %u\n", pcieGen);
		printw("PCIe Max Link gen : %u\n", maxGen);
		printw("Memory : %.3f / %.3f GiB\n", used, total);
		printw("Device file path: /dev/nvidia%u\n", pathNum);
		printw("GPU arch : %s\n", getArchName(arch));
		
		printw("CUDA Driver Version: %d\n", cuda / 1000);
		printw("NVIDIA Driver Version: %s\n", driver);
		printw("NVML Version: %s\n", nvml);

		printw("\nPress Q to quit\n");

		refresh();

		sleep(5);
	}
	
	endwin();
	nvmlShutdown();
}

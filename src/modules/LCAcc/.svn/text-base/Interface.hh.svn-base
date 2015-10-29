#ifndef LCACC_INTERFACE_H
#define LCACC_INTERFACE_H

struct LCAccDeviceHandle
{
	int value;
	int handleID;
	int netHandle;
	int deviceID;
};

class LCAccInterface
{
public:
        void (*ProcessMessage)(void*, int, int, const char*, int);
        int (*GetOpCode)(const char*);
	const char* (*GetOpName)(int);
	void (*GetOpTiming)(int opcode, unsigned int* ii, unsigned int* pipeDepth, unsigned int* cycleMult);
	void (*AddOperatingMode)(LCAccDeviceHandle* handle, const char* name);
	void (*SetNetPort)(LCAccDeviceHandle* handle, int port, int device);
	void (*SetTiming)(LCAccDeviceHandle* handle, unsigned int ii, unsigned int pipeline, unsigned int cycleMult);
	void (*SetPrefetchDistance)(LCAccDeviceHandle* handle, int distance);
	void (*SetSPMConfig)(LCAccDeviceHandle* handle, int banks, int bankMod, int readPorts, int readLatency, int writePorts, int writeLatency);
	void (*Initialize)(LCAccDeviceHandle* handle, uint32_t threadID);
	bool (*IsIdle)(LCAccDeviceHandle* handle);
	void (*HookToMemoryDevice)(LCAccDeviceHandle* handle, const char* name);
};

#endif

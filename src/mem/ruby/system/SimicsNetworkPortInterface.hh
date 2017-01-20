#ifndef RUBY_SIMICS_NETWORK_PORT_INTERFACE
#define RUBY_SIMICS_NETWORK_PORT_INTERFACE

typedef struct SimicsNetworkPortInterface_t
{
        int (*GetSimicsPortCount)();
	void (*BindDeviceToPort)(int portID, int deviceID);
	void (*UnbindDevice)(int deviceID);
	void (*SendMessageOnDevice)(int source, int target, const void* buffer, int length);
        void (*SendMessageOnPort)(int source, int target, const void* buffer, int length);
	int (*RegisterRecvHandlerOnDevice)(int device, void (*handler)(void*, int, int, const char*, int), void* data);
        int (*RegisterRecvHandlerOnPort)(int portID, void (*handler)(void*, int, int, const char*, int), void* data);
	void* (*UnregisterRecvHandler)(int handle);
}SimicsNetworkPortInterface;

#endif

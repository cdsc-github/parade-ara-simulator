#ifndef CFU_IDENTIFIER_H
#define CFU_IDENTIFIER_H

#include <stdint.h>

class CFUIdentifier
{
public:
	unsigned int cfuID;
	unsigned int cpuID;
	unsigned int acceleratorID;
	unsigned int owningThread;
	inline CFUIdentifier()
	{
		cfuID = (unsigned int)-1;
		cpuID = (unsigned int)-1;
		acceleratorID = (unsigned int)-1;
		owningThread = 0;
	}
};

#endif

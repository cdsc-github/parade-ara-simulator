#ifndef __LWI_INT
#define __LWI_INT
#define MAX_ACC_NUMBER 50000
#define MAX_LWI_ARG_SIZE 64
#include "../../mem/ruby/common/TypeDefines.hh"
#include <stdint.h>
typedef struct lwi_table
{
        unsigned int lwi_en;
        physical_address_t args_address;
	logical_address_t la_args;
} lwi_table_t;

typedef struct lwInt_ifc
{
	void (*raiseLightWeightInt)(int thread, void* args, int argSize, int interrupting_lcacc);
	int (*isReady)(int thread);//call first
} lwInt_ifc_t;

extern "C"
{
        void raiseLightWeightInt(int thread, void* args, int argSize, int interrupting_lcacc);
        int isReady(int thread);
}
#endif 


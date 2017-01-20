
/*
    Copyright (C) 1999-2008 by Mark D. Hill and David A. Wood for the
    Wisconsin Multifacet Project.  Contact: gems@cs.wisc.edu
    http://www.cs.wisc.edu/gems/

    --------------------------------------------------------------------

    This file is part of the Ruby Multiprocessor Memory System Simulator,
    a component of the Multifacet GEMS (General Execution-driven
    Multiprocessor Simulator) software toolset originally developed at
    the University of Wisconsin-Madison.

    Ruby was originally developed primarily by Milo Martin and Daniel
    Sorin with contributions from Ross Dickson, Carl Mauer, and Manoj
    Plakal.

    Substantial further development of Multifacet GEMS at the
    University of Wisconsin was performed by Alaa Alameldeen, Brad
    Beckmann, Jayaram Bobba, Ross Dickson, Dan Gibson, Pacia Harper,
    Derek Hower, Milo Martin, Michael Marty, Carl Mauer, Michelle Moravan,
    Kevin Moore, Andrew Phelps, Manoj Plakal, Daniel Sorin, Haris Volos,
    Min Xu, and Luke Yen.
    --------------------------------------------------------------------

    If your use of this software contributes to a published paper, we
    request that you (1) cite our summary paper that appears on our
    website (http://www.cs.wisc.edu/gems/) and (2) e-mail a citation
    for your published paper to gems@cs.wisc.edu.

    If you redistribute derivatives of this software, we request that
    you notify us and either (1) ask people to register with us at our
    website (http://www.cs.wisc.edu/gems/) or (2) collect registration
    information and periodically send it to us.

    --------------------------------------------------------------------

    Multifacet GEMS is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    Multifacet GEMS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Multifacet GEMS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA

    The GNU General Public License is contained in the file LICENSE.

### END HEADER ###
*/

/*------------------------------------------------------------------------*/
/* Includes                                                               */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/* Macro declarations                                                     */
/*------------------------------------------------------------------------*/

#ifndef _MF_MEMORY_API_H_
#define _MF_MEMORY_API_H_

#define SIM_ENHANCE
#define SIM_NET_PORTS
#define SIM_MEMORY

typedef struct CacheConfig
{
    int valid;
    unsigned numSets;
    unsigned blockSize;
    unsigned associativity;
} CacheConfigStruct;

typedef struct BiNBufferRequest_t
{
    unsigned int numberOfPoints;
    unsigned int* bufferSize;
    unsigned int* bandwidthRequirement;
    unsigned int* cacheImpact;
    int nodeID;
    int bufferID;
} BiNBufferRequest;

void scheduleCB(void (*)(void*), void*, uint64_t);
uint64_t GetSystemTime();

#ifdef SIM_MEMORY
#define SIM_MEMORY_READ 0
#define SIM_MEMORY_WRITE 1
#define SIM_MEMORY_SPM_READ 2
#define SIM_MEMORY_SPM_WRITE 3
#define SIM_MEMORY_BIC_READ 4
#define SIM_MEMORY_BIC_READ_COPY 5
#define SIM_MEMORY_BIC_WRITE 6

typedef struct SimicsMemoryInterfaceMemRequest_t
{
    int responseNeeded;//indicates that a response to this message is being solicited.  Not responding to this message is an error.  0 implies 'no', otherwise 'yes'
    uint64_t requestID;//A unique ID marking this request
    uint64_t emitTime;//time the request was originated, measured in cycles.  This is the time that a CPU emitted the request
    int source;//device source.  This is the node ID of the source from enumerated CPUs or accelerators
    int sourceType;//machine type enum work-around
    int target;
    int targetType;
    uint64_t sourcePC;//program counter
    int priority;//zero-normalized priority.  priority of zero is 'normal', higher is better.
    uint64_t logicalAddr;//logical address
    uint64_t physicalAddr;//physical address
    uint64_t size;//access size of this request.
    int bufferID;
    int bufferSize;
    int type;//marks the access as a read/write/eviction.
    int solicitingDeviceID;
} SimicsMemoryInterfaceMemRequest;

typedef struct SimicsMemoryInterfaceMemResponse_t
{
    uint64_t requestID;//relates to an ID associated with MemRequest
    int priority;
    int source;
    int sourceType;
    int target;
    int targetType;
    uint64_t logicalAddr;
    uint64_t physicalAddr;
    uint64_t size;
    int bufferID;
    int bufferSize;
    int type;
} SimicsMemoryInterfaceMemResponse;

typedef struct mf_simics_memory_interface_api_x
{
    void (*RegisterMemMsgReciever)(void* obj, int controller, void (*handler)(const SimicsMemoryInterfaceMemRequest* mReq, void* args), void* args);
    void (*EmitMemMsgResponse)(void* obj, int controller, const SimicsMemoryInterfaceMemResponse* mResp, unsigned int delay);
    int (*GetControllerCount)(void* obj);
    uint64_t (*CurrentTime)(void* obj);
} mf_simics_memory_interface_api;
#endif

#endif //_MF_MEMORY_API_H_

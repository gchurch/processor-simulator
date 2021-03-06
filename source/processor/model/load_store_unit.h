//===========================
//include guard
#ifndef LOAD_STORE_UNIT_H
#define LOAD_STORE_UNIT_H

//===========================
//forward declared dependencies
class Memory;
class ReorderBuffer;
class ALUReservationStation;

//=================================
// included dependencies
#include "store_buffer.h"
#include "load_buffer.h"
#include "constants.h"

//===========================
//class declaration
class LoadStoreUnit {

  //private member variables
  private:

    //forward components
    Memory* const memory;
    ReorderBuffer* const reorderBuffer;

    //decoded instruction
    int nextLoadOpcode;
    int nextLoadOperands[numOfOperands];
    int nextLoadReorderBufferIndex;

    int nextStoreOpcode;
    int nextStoreOperands[numOfOperands];
    int nextStoreReorderBufferIndex;

    int loadOpcode;
    int loadOperands[numOfOperands];
    int loadReorderBufferIndex;

    int storeOpcode;
    int storeOperands[numOfOperands];
    int storeReorderBufferIndex;

    const int bufferSize;
    const int writeCycles;
    const int readCycles;

    //Read and write buffers to store load and store instruciton in operation
    StoreBuffer storeBuffer;
    LoadBuffer loadBuffer;

    ALUReservationStation* const aluReservationStation;

  //public functions
  public:

    LoadStoreUnit(Memory* const memory, ReorderBuffer* const reorderBuffer, 
      ALUReservationStation* const aluReservationStation);

    void execute();

    void pipe();

    void flush();

  //getters and setters
  public:

    void setNextLoadOpcode(const int x);

    void setNextLoadOperands(const int x[numOfOperands]);

    void setNextLoadReorderBufferIndex(const int i);

    void setNextStoreOpcode(const int x);

    void setNextStoreOperands(const int x[numOfOperands]);

    void setNextStoreReorderBufferIndex(const int i);
};

#endif
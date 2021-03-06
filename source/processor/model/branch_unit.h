//===========================
//include guard
#ifndef BRANCH_UNIT_H
#define BRANCH_UNIT_H

//===========================
//forward declared dependencies
class ReorderBuffer;

//===========================================
// included dependencies
#include "constants.h"

//===========================
//class declaration
class BranchUnit {

  //private member variables
  private:
	
    ReorderBuffer* const reorderBuffer;

    //decoded information
    int nextOpcode;
    int nextOperands[numOfOperands];
    int nextReorderBufferIndex;

    //next information
    int opcode;
    int operands[numOfOperands];
    int reorderBufferIndex;

    //branch outcome
    bool successful;

  //public functions
  public:
    BranchUnit(ReorderBuffer* const reorderBuffer);

    void execute();

    void pipe();

    void flush();

  //getters and setters
  public:

    void setNextOpcode(const int x);

    void setNextOperands(const int x[numOfOperands]);

    void setNextReorderBufferIndex(const int i);

    bool getSuccessful() const;

    int getReorderBufferIndex() const;
};

#endif
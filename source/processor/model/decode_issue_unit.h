//===========================
//include guard
#ifndef DECODE_ISSUE_UNIT_H
#define DECODE_ISSUE_UNIT_H

//===========================
//forward declared dependencies
class RegisterFile;
class ReorderBuffer;
class ALUReservationStation;
class BranchUnitReservationStation;
class LoadStoreUnitReservationStation;
class LoadStoreUnit;
class LoadQueue;
class StoreQueue;

//=================================
// included dependencies
#include "instructions.h"
#include "constants.h"

//===========================
//class declaration
class DecodeIssueUnit {

  //private member variables
  private:
    
    //forward components
    RegisterFile* const registerFile;
    ReorderBuffer* const reorderBuffer;
    ALUReservationStation* const aluReservationStation;
    BranchUnitReservationStation* const branchUnitReservationStation;
    StoreQueue* const storeQueue;
    LoadQueue* const loadQueue;

    const int issueWindowSize;

    //Instruction given from the fetch unit
    Instruction* const nextInstructions;
    int* const nextBranchAddresses;

    Instruction* const instructions;
    OperandType** const operandTypes;
    int* const branchAddresses;
    bool* const instructionsIssued;
    int* const reorderBufferIndexes;

    const bool branchPrediction;

  //public functions
  public:
    DecodeIssueUnit(RegisterFile* const registerFile, ReorderBuffer* const reorderBuffer, 
      ALUReservationStation* const aluReservationStation, 
      BranchUnitReservationStation* const branchUnitReservationStation, 
      StoreQueue* const storeQueue, LoadQueue* const loadQueue, const int issueWindowSize, 
      const bool branchPrediction);

    void execute();

    void issue(int instructionToIssue);

    void pipe();

    void flush();

    void print() const;

    bool allInstructionsIssued() const;

    int numFreeSpaces() const;

  //private instructins
  private:

    void setOperandTypes(const int index);

    void pipeInstruction(int instructionToIssue);

    void moveInstructions();

  //getters and setters
  public:

    void setNextInstructions(const Instruction x[]);

    void getInstructions(Instruction copy[]) const;

    void getReorderBufferIndexes(int copy[]) const;

    void setNextBranchAddresses(const int x[]);

    void getOperandTypes(OperandType copy[][numOfOperands]) const;
};

#endif
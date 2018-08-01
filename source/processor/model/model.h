//===========================
//include guard
#ifndef MODEL_H
#define MODEL_H

//=================================
// included dependencies
#include "instructions.h"
#include "register_file.h"
#include "memory.h"
#include "reorder_buffer.h"
#include "fetch_unit.h"
#include "decode_issue_unit.h"
#include "alu.h"
#include "alu_reservation_station.h"
#include "branch_unit.h"
#include "branch_unit_reservation_station.h"
#include "load_store_unit.h"
#include "load_store_unit_reservation_station.h"

//===========================
//class declaration
class Model {

  private:

    //Instructions to execute
    const Instructions instructions;

    //general stats
    int noOfInstructionsExecuted;
    int noOfClockCycles;
    float noOfInstructionsPerCycle;

    //processor configuration
    const int numArchitecturalRegisters;
    const int numPhysicalRegisters;
    const int memorySize;
    const int issueWindowSize;
    const int aluReservationStationSize;
    const int branchUnitReservationStationSize;
    const int loadStoreUnitReservationStationSize;
    const int reorderBufferSize;
    const int numReorderBufferFields;

    //registers
    int pc;

    //status flags
    int flushFlag;
    int runningFlag;

    //components
    RegisterFile registerFile;
    Memory memory;
    ReorderBuffer reorderBuffer;
    FetchUnit fetchUnit;
    DecodeIssueUnit decodeIssueUnit;
    ALU alu;
    ALUReservationStation aluReservationStation;
    BranchUnit branchUnit;
    BranchUnitReservationStation branchUnitReservationStation;
    LoadStoreUnit loadStoreUnit;
    LoadStoreUnitReservationStation loadStoreUnitReservationStation;

  public:
    Model(const Instructions instructions);

    void updateStats();

    void cycle();

    void run();

    void fetch();

    void decodeIssue();

    void dispatch();

    void execute();

    void commit();

    void pipe();

    void flushPipeline();

    void printInfo() const;

    //===============================
    // getter functions

    int getNumRegisters() const;

    int getMemorySize() const;

    int getIssueWindowSize() const;

    int getRunningFlag() const;

    int getNoOfInstructionsExecuted() const;

    int getNoOfClockCycles() const;

    float getNoOfInstructionsExecutedPerCycle() const;

    int getPC() const;

    void getRenameTable(int* const copy) const;

    void getArchitecturalRegisterValues(int* const copy) const;

    void getLatestArchitecturalRegisterValues(int* const copy) const;

    void getAllMemoryValues(int* const copy) const;

    Instruction getFetchUnitInstruction() const;

    void getFetchUnitInstructions(Instruction* const copy) const;

    void getDecodeIssueUnitInstructions(Instruction* const copy) const;

    void getDecodeIssueUnitReorderBufferIndexes(int* const copy) const;

    int getAluReservationStationSize() const;

    void getAluReservationStationInstructions(Instruction* const copy) const;

    void getAluReservationStationReorderBufferIndexes(int* const copy) const;

    int getBranchUnitReservationStationSize() const;

    void getBranchUnitReservationStationInstructions(Instruction* const copy) const;

    void getBranchUnitReservationStationReorderBufferIndexes(int* const copy) const;

    int getLoadStoreUnitReservationStationSize() const;

    void getLoadStoreUnitReservationStationInstructions(Instruction* const copy) const;

    void getLoadStoreUnitReservationStationReorderBufferIndexes(int* const copy) const;

    int getAluResult() const;

    int getReorderBufferSize() const;

    int getNumReorderBufferFields() const;

    int getAluReorderBufferIndex() const;

    bool getBranchUnitSuccessful() const;

    int getBranchUnitReorderBufferIndex() const;

    void getReorderBufferInstructions(Instruction* const copy) const;

    void getReorderBufferFields(int** const copy) const;

    int getReorderBufferTailIndex() const;

    int getReorderBufferHeadIndex() const;
};

#endif
//===========================================
//included header file containing interface
#include "decode_issue_unit.h"

//===========================================
// included dependencies
#include <iostream>

#include "register_file.h"
#include "reorder_buffer.h"
#include "alu_reservation_station.h"
#include "branch_unit_reservation_station.h"
#include "load_store_unit_reservation_station.h"
#include "instructions.h"

//===========================================
//class implementation

DecodeIssueUnit::DecodeIssueUnit(RegisterFile* registerFile, ReorderBuffer* reorderBuffer, ALUReservationStation* aluReservationStation, BranchUnitReservationStation* branchUnitReservationStation, LoadStoreUnitReservationStation* loadStoreUnitReservationStation, int* blockingFlag) :
  registerFile(registerFile),
  reorderBuffer(reorderBuffer),
  aluReservationStation(aluReservationStation),
  branchUnitReservationStation(branchUnitReservationStation),
  loadStoreUnitReservationStation(loadStoreUnitReservationStation),
  nextInstruction((Instruction) {0,0,0,0}),
  currentInstruction((Instruction) {0,0,0,0}),
  reorderBufferIndex(-1),
  blockingFlag(blockingFlag)
{}

void DecodeIssueUnit::execute() {
  if(nextInstruction.opcode != 0) {
    currentInstruction = nextInstruction;
    issue();
  }
}

void DecodeIssueUnit::issue() {
  switch(currentInstruction.opcode) {

    //ALU instructions
    case ADD:
    case ADDI:
    case AND:
    case MULT:
    case OR:
    case SUB:
      //if there is space in the reservation then issue the instruction
      if(aluReservationStation->findFreePosition() != -1 && registerFile->getScoreBoardValue(currentInstruction.operands[0])) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_REGISTER, currentInstruction.operands[0], currentInstruction);
        //Set the scoreboard value of the destination register to zero
        registerFile->setScoreBoardValue(currentInstruction.operands[0],0);
        //unblock the pipeline
        *blockingFlag = 0;
      }
      //otherwise block the pipeline
      else {
        *blockingFlag = 1;
      }
      break;

    //Load Store unit instructions
    case LW:
    case LWR:
      if(loadStoreUnitReservationStation->spaceInQueue() && registerFile->getScoreBoardValue(currentInstruction.operands[0])) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_REGISTER, currentInstruction.operands[0], currentInstruction);
        //Set the scoreboard value of the destination register to zero
        registerFile->setScoreBoardValue(currentInstruction.operands[0],0);
        //unblock the pipeline
        *blockingFlag = 0;
      }
      else {
        *blockingFlag = 1;
      }
      break;
    case SW:
    case SWR:
      if(loadStoreUnitReservationStation->spaceInQueue()) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_MEMORY, currentInstruction.operands[1], currentInstruction);
        *blockingFlag = 0;
      }
      else {
        *blockingFlag = 1;
      }
      break;

    //Branch unit instructions
    case BEQ:
    case BNE:
      if(branchUnitReservationStation->findFreePosition() != -1) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(JUMP, currentInstruction.operands[2], currentInstruction);
        *blockingFlag = 0;
      }
      else {
        *blockingFlag = 1;
      }
      break;
    case BGEZ:
    case BGTZ:
    case BLEZ:
    case BLTZ:
      break;
    case J:
    case JR:
      if(branchUnitReservationStation->findFreePosition() != -1) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(JUMP, currentInstruction.operands[0], currentInstruction);
        *blockingFlag = 0;
      }
      else {
        *blockingFlag = 1;
      }
      break;
                    
    //Instruction to finish the program
    case HALT:
      if(branchUnitReservationStation->findFreePosition() != -1) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(SYSCALL, currentInstruction.operands[0], currentInstruction);
        *blockingFlag = 0;
      }
      else {
        *blockingFlag = 1;
      }
      break;
  }
}

void DecodeIssueUnit::pipe() {
  if(reorderBufferIndex != -1) {
    switch(currentInstruction.opcode) {

      //ALU instructions
      case ADD:
      case ADDI:
      case AND:
      case MULT:
      case OR:
      case SUB:
        aluReservationStation->addInstruction(currentInstruction, reorderBufferIndex);
        break;

      //Load Store unit instructions
      case LW:
      case LWR:
      case SW:
      case SWR:
        loadStoreUnitReservationStation->addInstruction(currentInstruction, reorderBufferIndex);
        break;

      //Branch unit instructions
      case BEQ:
      case BNE:
      case BGEZ:
      case BGTZ:
      case BLEZ:
      case BLTZ:
      case J:
      case JR:

      //Instruction to finish the program
      case HALT:
        branchUnitReservationStation->addInstruction(currentInstruction, reorderBufferIndex);
        break;
    }
    reorderBufferIndex = -1;
    nextInstruction = (Instruction) {0,0,0,0};
  }
}

void DecodeIssueUnit::print() {
  std::cout << "DECODE ISSUE UNIT:";
  printInstruction(currentInstruction);
}

void DecodeIssueUnit::setNextInstruction(Instruction x) {
  nextInstruction = x;
}

void DecodeIssueUnit::flush() {
  nextInstruction = (Instruction) {0,0,0,0};
  currentInstruction = (Instruction) {0,0,0,0};
}
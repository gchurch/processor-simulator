//===========================================
//included header file containing interface
#include "decode_issue_unit.h"

//===========================================
// included dependencies
#include <stdio.h>

#include "register_file.h"
#include "reorder_buffer.h"
#include "alu_reservation_station.h"
#include "branch_unit_reservation_station.h"
#include "load_store_unit_reservation_station.h"
#include "instructions.h"

//===========================================
//class implementation

DecodeIssueUnit::DecodeIssueUnit(RegisterFile* const registerFile, ReorderBuffer* const reorderBuffer, ALUReservationStation* const aluReservationStation, BranchUnitReservationStation* const branchUnitReservationStation, LoadStoreUnitReservationStation* const loadStoreUnitReservationStation) :
  registerFile(registerFile),
  reorderBuffer(reorderBuffer),
  aluReservationStation(aluReservationStation),
  branchUnitReservationStation(branchUnitReservationStation),
  loadStoreUnitReservationStation(loadStoreUnitReservationStation),
  numInstructions(4),
  nextInstructions(new Instruction[numInstructions]),
  currentInstructions(new Instruction[numInstructions]),
  currentInstructionsIssued(new bool[numInstructions]),
  reorderBufferIndex(-1),
  blockingFlag(false)
{
  for(int i = 0; i < numInstructions; i++) {
    nextInstructions[i] = (Instruction) {0,0,0,0};
    currentInstructions[i] = (Instruction) {0,0,0,0};
    currentInstructionsIssued[i] = false;
  }
}

void DecodeIssueUnit::execute() {
  if(reorderBuffer->freeSpace()) {
    issue(0);
  }
  else {
    blockingFlag = true;
  }
}

void DecodeIssueUnit::issue(int instructionToIssue) {
  switch(currentInstructions[instructionToIssue].opcode) {

    //NOOP instruction
    case NOOP:
      break;

    //ALU instructions
    case ADDI:
      //if there is space in the reservation then issue the instruction
      if(aluReservationStation->freeSpace()) {
        //get the destination architectural register
        int architecturalRegister = currentInstructions[instructionToIssue].operands[0];

        //get the current mapping of the destination architectural register
        int oldPhysicalRegister = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0]);

        //find a free register to use as the destination
        int newPhysicalRegister = registerFile->findFreePhysicalRegister();
        registerFile->usePhysicalRegister(newPhysicalRegister);

        //Add instruction to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_REGISTER, 0, architecturalRegister, newPhysicalRegister, 
          oldPhysicalRegister, currentInstructions[instructionToIssue]);

        //rename the source operands
        currentInstructions[instructionToIssue].operands[1] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[1]);

         //update the rename table
        registerFile->setArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0], newPhysicalRegister);

        //rename destination operand
        currentInstructions[instructionToIssue].operands[0] = newPhysicalRegister;

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;

      }
      //otherwise block the pipeline
      else {
        blockingFlag = true;
      }
      break;

    case ADD:
    case AND:
    case MULT:
    case OR:
    case SUB:

      //if there is space in the reservation then issue the instruction
      if(aluReservationStation->freeSpace()) {
        //get the destination architectural register
        int architecturalRegister = currentInstructions[instructionToIssue].operands[0];

        //get the current mapping of the destination architectural register
        int oldPhysicalRegister = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0]);

        //find a free register to use as the destination
        int newPhysicalRegister = registerFile->findFreePhysicalRegister();
        registerFile->usePhysicalRegister(newPhysicalRegister);

        //Add instruction to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_REGISTER, 0, architecturalRegister, newPhysicalRegister, 
          oldPhysicalRegister, currentInstructions[instructionToIssue]);

        //rename the source operands
        currentInstructions[instructionToIssue].operands[1] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[1]);
        currentInstructions[instructionToIssue].operands[2] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[2]);

        //update the rename table
        registerFile->setArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0], newPhysicalRegister);

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //rename the destination operand
        currentInstructions[instructionToIssue].operands[0] = newPhysicalRegister;

        //unblock the pipeline
        blockingFlag = false;
      }
      //otherwise block the pipeline
      else {
        blockingFlag = true;
      }
      break;

    //Load Store unit instructions
    case LW:
      if(loadStoreUnitReservationStation->freeSpace()) {
        //get the destination architectural register
        int architecturalRegister = currentInstructions[instructionToIssue].operands[0];

        //get the current mapping of the destination architectural register
        int oldPhysicalRegister = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0]);

        //find a free register to use as the destination
        int newPhysicalRegister = registerFile->findFreePhysicalRegister();
        registerFile->usePhysicalRegister(newPhysicalRegister);

        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_REGISTER, 0, architecturalRegister, newPhysicalRegister, 
          oldPhysicalRegister, currentInstructions[instructionToIssue]);
        
        //update the rename table
        registerFile->setArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0], newPhysicalRegister);

        //rename the destination operand
        currentInstructions[instructionToIssue].operands[0] = newPhysicalRegister;

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;
      }
      else {
        //block the pipeline
        blockingFlag = true;
      }
      break;

    case LWR:
      if(loadStoreUnitReservationStation->freeSpace()) {
        //get the destination architectural register
        int architecturalRegister = currentInstructions[instructionToIssue].operands[0];

        //get the current mapping of the destination architectural register
        int oldPhysicalRegister = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0]);

        //find a free register to use as the destination
        int newPhysicalRegister = registerFile->findFreePhysicalRegister();
        registerFile->usePhysicalRegister(newPhysicalRegister);

        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_REGISTER, 0, architecturalRegister, newPhysicalRegister, 
          oldPhysicalRegister, currentInstructions[instructionToIssue]);

        //rename the source operands
        currentInstructions[instructionToIssue].operands[1] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[1]);

        //update the rename table
        registerFile->setArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0], newPhysicalRegister);

        //rename destination operand
        currentInstructions[instructionToIssue].operands[0] = newPhysicalRegister;

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;
      }
      else {
        //block the pipeline
        blockingFlag = true;
      }
      break;

    case SW:
      if(loadStoreUnitReservationStation->freeSpace()) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_MEMORY, 0, 0, 0, 0, currentInstructions[instructionToIssue]);

        //rename the registers
        currentInstructions[instructionToIssue].operands[0] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0]);

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;
      }
      else {
        //block the pipeline
        blockingFlag = true;
      }
      break;

    case SWR:
      if(loadStoreUnitReservationStation->freeSpace()) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(STORE_TO_MEMORY, 0, 0, 0, 0, currentInstructions[instructionToIssue]);

        //rename the registers
        currentInstructions[instructionToIssue].operands[0] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0]);
        currentInstructions[instructionToIssue].operands[1] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[1]);

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;
      }
      else {
        //block the pipeline
        blockingFlag = true;
      }
      break;

    //Branch unit instructions
    case BEQ:
    case BNE:
      if(branchUnitReservationStation->freeSpace()) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(JUMP, currentInstructions[instructionToIssue].operands[2], 0, 0, 0, currentInstructions[instructionToIssue]);

        //rename the registers
        currentInstructions[instructionToIssue].operands[0] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[0]);
        currentInstructions[instructionToIssue].operands[1] = registerFile->getArchitecturalRegisterMapping(currentInstructions[instructionToIssue].operands[1]);

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;
      }
      else {
        //block the pipeline
        blockingFlag = true;
      }
      break;
    case BGEZ:
    case BGTZ:
    case BLEZ:
    case BLTZ:
      break;
    case J:
    case JR:
      if(branchUnitReservationStation->freeSpace()) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(JUMP, currentInstructions[instructionToIssue].operands[0], 0, 0, 0, currentInstructions[instructionToIssue]);

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;
      }
      else {
        //block the pipeline
        blockingFlag = true;
      }
      break;
                    
    //Instruction to finish the program
    case HALT:
      if(branchUnitReservationStation->freeSpace()) {
        //Instruction has been issued so add entry to the reorder buffer
        reorderBufferIndex = reorderBuffer->addEntry(SYSCALL, 0, 0, 0, 0, currentInstructions[instructionToIssue]);

        //take note that the instruction was issued
        currentInstructionsIssued[instructionToIssue] = true;

        //unblock the pipeline
        blockingFlag = false;
      }
      else {
        //block the pipeline
        blockingFlag = true;
      }
      break;
  }
}

void DecodeIssueUnit::pipe() {
  if(!blockingFlag) {
  //send the current instruction to the necessary component
  switch(currentInstructions[0].opcode) {

    //NOOP instruction
    case NOOP:
      break;

    //ALU instructions
    case ADD:
    case ADDI:
    case AND:
    case MULT:
    case OR:
    case SUB:
      //Set the scoreboard value of the destination register to zero
      registerFile->setScoreBoardValue(currentInstructions[0].operands[0],0);
      aluReservationStation->setNextInstruction(currentInstructions[0]);
      aluReservationStation->setNextReorderBufferIndex(reorderBufferIndex);
      break;

    //Load Store unit instructions
    case LW:
    case LWR:
      //Set the scoreboard value of the destination register to zero
      registerFile->setScoreBoardValue(currentInstructions[0].operands[0],0);
    case SW:
    case SWR:
      loadStoreUnitReservationStation->setNextInstruction(currentInstructions[0]);
      loadStoreUnitReservationStation->setNextReorderBufferIndex(reorderBufferIndex);
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
      //branchUnitReservationStation->addInstruction(currentInstruction, reorderBufferIndex);
      branchUnitReservationStation->setNextInstruction(currentInstructions[0]);
      branchUnitReservationStation->setNextReorderBufferIndex(reorderBufferIndex);
      break;
  }
  //set the current instruction equal to the next instruction
  currentInstructions[0] = nextInstructions[0];
  //clear the nextInstruction
  nextInstructions[0] = (Instruction) {0,0,0,0};
}
}

void DecodeIssueUnit::flush() {
  for(int i = 0; i < numInstructions; i++) {
    nextInstructions[i] = (Instruction) {0,0,0,0};
    currentInstructions[i] = (Instruction) {0,0,0,0};
    currentInstructionsIssued[i] = false;
  }
  reorderBufferIndex = -1;
  blockingFlag = false;
}

void DecodeIssueUnit::print() const {
  printf("DECODE ISSUE UNIT:");
  printInstruction(currentInstructions[0]);
}

bool DecodeIssueUnit::allInstructionsIssued() const {
  for(int i = 0; i < numInstructions; i++) {
    if(currentInstructions[i].opcode != NOOP && !currentInstructionsIssued[i]) {
      return false;
    }
  }
  return true;
}

//======================================================================
//getters and setters

void DecodeIssueUnit::setNextInstructions(const Instruction* const x) {
  for(int i = 0; i < numInstructions; i++) {
    nextInstructions[i] = x[i];
  }
}

Instruction DecodeIssueUnit::getCurrentInstruction() const {
  return currentInstructions[0];
}

bool DecodeIssueUnit::getBlockingFlag() const {
  return blockingFlag;
}
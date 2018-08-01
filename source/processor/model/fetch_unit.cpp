//===========================================
//included header file containing interface
#include "fetch_unit.h"

//===========================================
// included dependencies
#include <stdio.h>

#include "decode_issue_unit.h"

//===========================================
//class implementation

FetchUnit::FetchUnit(const Instructions instructions, int* const pc, DecodeIssueUnit* const decodeIssueUnit, 
  const int issueWindowSize) :
  instructions(instructions),
  pc(pc),
  decodeIssueUnit(decodeIssueUnit),
  issueWindowSize(issueWindowSize),
  currentInstructions(new Instruction[issueWindowSize])
{
  //initialise all instructions to NOOPs
  for(int i = 0; i < issueWindowSize; i++) {
    currentInstructions[i] = (Instruction) {0,0,0,0};
  }
}

void FetchUnit::fetchInstructions() {
  for(int i = 0; i < issueWindowSize; i++) {
    if(*pc + i < instructions.getNumOfInstructions()) {
      currentInstructions[i] = instructions.at(*pc + i);
    }
    else {
      currentInstructions[i] = (Instruction) {0,0,0,0};
    }
  }
}

void FetchUnit::execute() {
  if(decodeIssueUnit->allInstructionsIssued()) {
    if(*pc < instructions.getNumOfInstructions()) {
      //fetch instructions
      fetchInstructions();
      //increment the program counter
      (*pc) += issueWindowSize;
    }
    else {
      //next instruction is noop if pc exceeds number of instructions
      currentInstructions[0] = (Instruction) {0,0,0,0};
    }
  }
}

void FetchUnit::print() const {
  printf("FETCHED INSTRUCTION: ");
  printInstruction(currentInstructions[0]);
}

void FetchUnit::pipe() {
  if(decodeIssueUnit->allInstructionsIssued()) {
    //put the fetched instruction into the instruction register
    decodeIssueUnit->setNextInstructions(currentInstructions);
    for(int i = 0; i < issueWindowSize; i++) {
      currentInstructions[i] = (Instruction) {0,0,0,0};
    }
  }
}

void FetchUnit::flush() {
  currentInstructions[0] = (Instruction) {0,0,0,0};
}

//======================================
// getter functions

Instruction FetchUnit::getCurrentInstruction() const {
  return currentInstructions[0];
}

void FetchUnit::getCurrentInstructions(Instruction* const copy) const {
  for(int i = 0; i < issueWindowSize; i++) {
    copy[i] = currentInstructions[i];
  }
}
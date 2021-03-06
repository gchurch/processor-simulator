//===========================
//include guard
#ifndef CONSTANTS_H
#define CONSTANTS_H

//instruction opcodes
enum Opcodes {
  NOOP,
  ADD,
  ADDI,
  AND,
  MULT,
  OR,
  SUB,
  LW,
  LWR,
  SW,
  SWR,
  BEQ,
  BGEZ,
  BGTZ,
  BLEZ,
  BLTZ,
  BNE,
  J,
  JR,
  HALT
};

//type in the reorder buffer
enum Type {
  BRANCH,
  STORE_TO_MEMORY,
  STORE_TO_REGISTER,
  SYSCALL
};

//index in the reorder buffer
enum ReorderBufferIndex {
  TYPE,
  STATUS,
  DESTINATION_REGISTER,
  RESULT,
  BRANCH_PREDICTION,
  BRANCH_TARGET_ADDRESS,
  COUNT
};

//status in the reorder buffer
enum Status {
  EMPTY,
  ISSUED,
  EXECUTING,
  FINISHED
};

enum OperandType {
  NONE,
  REGISTER,
  ROB,
  CONSTANT
};

const int numOfOperands = 3;

#endif
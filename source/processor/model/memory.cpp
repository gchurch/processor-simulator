//===========================================
//included header file containing interface
#include "memory.h"

//===========================================
// included dependencies
#include <stdio.h>

//===========================================
//class implementation

Memory::Memory(int size) : size(size) {
  memory = new int[size];
  for(int i = 0; i < size; i++) {
    memory[i] = 0;
  }
}

void Memory::getAllMemoryValues(int* copy) {
  for(int i = 0; i < size; i++) {
    copy[i] = memory[i];
  }
}

int Memory::loadFromMemory(int address) {
  if(address < 0 || address > size - 1) {
    printf("Register index %d is out of bounds.\n", address);
  }
  return memory[address];
}

void Memory::storeInMemory(int address, int value) {
  if(address < 0 || address > size - 1) {
    printf("Register index %d is out of bounds.\n", address);
  }
  memory[address] = value;
}

void Memory::print() {
  printf("Memory:\n");
  for(int i = 0; i < size; i++) {
    printf("%d ", memory[i]);
  }
  printf("\n");
}
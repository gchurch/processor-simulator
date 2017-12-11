#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>

#include "instructions.h"
#include "register_file.h"
#include "memory.h"
#include "alu.h"
#include "branch_unit.h"
#include "memory_unit.h"
#include "decode_unit.h"
#include "fetch_unit.h"


class Processor {

    //information
    int noOfRegisters;
    int memorySize;

    //general stats
    int noOfInstructionsExecuted;
    int noOfClockCycles;

    //Instruction info
    int noOfInstructions;
    Instruction *instructions;

    //registers
    int pc;

    //status flags
    int flushFlag;

    //components
    RegisterFile registerFile;
    Memory memory;
    FetchUnit fetchUnit;
    DecodeUnit decodeUnit;
    ALU alu;
    BranchUnit branchUnit;
    MemoryUnit memoryUnit;

    public:
    	//Classes needed to be initialised in the uniform initialiser list
        Processor(int noOfRegisters, int memorySize, int noOfInstructions, Instruction *instructions) : 

            //processor configuration
            noOfRegisters(noOfRegisters),
            memorySize(memorySize),

            //general stats
            noOfInstructionsExecuted(0),
            noOfClockCycles(0),

            //instruction info
            noOfInstructions(noOfInstructions),
            instructions(instructions),

            //special registers
            pc(1),

            //status flags
            flushFlag(0),

            //components
            registerFile(noOfRegisters), 
            memory(memorySize),
            fetchUnit(instructions, noOfInstructions, &pc, &decodeUnit),
            decodeUnit(&registerFile, &alu, &branchUnit, &memoryUnit),
            alu(&registerFile),
            branchUnit(&pc, &flushFlag),
            memoryUnit(&memory, &registerFile)
        {}


        void start() {

            printf("Keep pressing ENTER to step through the program\n");

            printInfo();

            //step through the program
        	while(1) {

                //hold up the program at each clock cycle
                char str[3];
                fgets(str, 2, stdin);

                //run each unit
                alu.execute();
                branchUnit.execute();
                memoryUnit.execute();
                decodeUnit.execute();
                fetchUnit.execute();

                //propogate values through pipeline
                fetchUnit.pipe();
                decodeUnit.pipe();

                //check if we received a message to flush the pipeline
                if(flushFlag == 1) {
                    //if so then flush the pipeline
                    flushPipeline();
                }

                //update info
                noOfClockCycles++;

                //print register info
                printInfo();
            }
        }

        void flushPipeline() {
            alu.flush();
            branchUnit.flush();
            decodeUnit.flush();
            fetchUnit.flush();
            decodeUnit.flush();
            flushFlag = 0;
        }

        void printInfo() {
            printf("\n");
            printf("Number of clock cycles: %d\n", noOfClockCycles);
            printf("\n");
            printf("PC: %d\n", pc);
            registerFile.printRegisters();
            printf("\n");
        }
};

int main(int argc, char *argv[]) {
    printf("\n");

    //If command line arguments are incorrect then stop program
    if(argc != 2) {
        printf("Machine code file not valid.\n");
        return 1;
    }

    //get the instructions from the machine code file
    std::string inputFileName(argv[1]);
    int numOfInstructions = getNumOfInstructions(inputFileName);
    Instruction *instructions = NULL;
    if(numOfInstructions != -1) {
        instructions = getInstructions(inputFileName, numOfInstructions);
    }

    //create processor object and start processing
    if(numOfInstructions != -1 && instructions != NULL) {
        Processor processor(16, 64, numOfInstructions,instructions);
        processor.start();
    }
    printf("\n");
    return 0;
}

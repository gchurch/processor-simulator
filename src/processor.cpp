#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <fstream>
#include <string>

#include "constants.h"
#include "instructions.h"
#include "register_file.h"
#include "memory.h"
#include "reorder_buffer.h"
#include "alu.h"
#include "branch_unit.h"
#include "store_queue.h"
#include "load_queue.h"
#include "load_store_unit.h"
#include "decode_unit.h"
#include "fetch_unit.h"
#include "alu_reservation_station.h"

//Had to do this to avoid errors from forward declaration
void ReorderBuffer::setHead(LoadStoreUnit* loadStoreUnit, int rbi) {
    loadStoreUnit->setHead(rbi);
}

using namespace std;


class Processor {

    //information
    int noOfRegisters;
    int memorySize;

    //general stats
    int noOfInstructionsExecuted;
    int noOfClockCycles;

    //Instructions to execute
    Instructions instructions;

    //registers
    int pc;

    //status flags
    int flushFlag;
    int runningFlag;
    int decodeUnitBlockingFlag;
    int loadStoreUnitBlockingFlag;

    //components
    RegisterFile registerFile;
    Memory memory;
    ReorderBuffer reorderBuffer;
    FetchUnit fetchUnit;
    DecodeUnit decodeUnit;
    ALU alu;
    BranchUnit branchUnit;
    LoadStoreUnit loadStoreUnit;

    public:
    	//Classes needed to be initialised in the uniform initialiser list
        Processor(Instructions instructions) : 

            //processor configuration
            noOfRegisters(16),
            memorySize(24),

            //general stats
            noOfInstructionsExecuted(0),
            noOfClockCycles(0),

            //instruction info
            instructions(instructions),

            //registers
            pc(1),

            //status flags
            flushFlag(0),
            runningFlag(1),
            decodeUnitBlockingFlag(0),
            loadStoreUnitBlockingFlag(0),

            //components
            registerFile(noOfRegisters), 
            memory(memorySize),
            reorderBuffer(&registerFile, &memory, &loadStoreUnit, &pc, &flushFlag, &runningFlag, &noOfInstructionsExecuted),
            fetchUnit(instructions, &pc, &decodeUnit),
            decodeUnit(&registerFile, &reorderBuffer, &alu, &branchUnit, &loadStoreUnit, &decodeUnitBlockingFlag),
            alu(&registerFile, &reorderBuffer),
            branchUnit(&reorderBuffer),
            loadStoreUnit(&memory, &reorderBuffer, &loadStoreUnitBlockingFlag)
        {}


        void start() {

            cout << "Keep pressing ENTER to step through the program\n";

            printInfo();

            //step through the program
        	while(runningFlag) {

                //hold up the program at each clock cycle
                char str[3];
                //fgets(str, 2, stdin);

                //if the pipeline is not being blocked
                if(!decodeUnitBlockingFlag && !loadStoreUnitBlockingFlag) {
                    //propogate values through pipeline
                    pipe();
                    //fetch the next instruction
                    fetch();
                }

                //decode the instruction
                decode();

                //execute the instruction
                execute();

                 //write the results to the reorder buffer
                writeResult();      

                //writeback the results
                commit();

                //check if we should flush the pipeline
                if(flushFlag == 1) {
                    flushPipeline();
                }

                //update info
                noOfClockCycles++;

                //print register info
                printInfo();
            }

            cout << "PROGRAM FINISHED\n";
        }

        void pipe() {
            fetchUnit.pipe();
            decodeUnit.pipe();
        }

        void fetch() {
            fetchUnit.execute();
        }

        void decode() {
            decodeUnit.execute();
        }

        void execute() {
            alu.execute();
            branchUnit.execute();
            loadStoreUnit.execute();
        }

        void writeResult() {
            alu.writeResult();
            loadStoreUnit.writeResult();
            branchUnit.writeResult();
        }

        void commit() {
            reorderBuffer.checkTailForStore();
            reorderBuffer.retire();
        }

        void flushPipeline() {
            alu.flush();
            branchUnit.flush();
            decodeUnit.flush();
            fetchUnit.flush();
            decodeUnit.flush();
            flushFlag = 0;
            registerFile.resetScoreBoard();
            reorderBuffer.flush();
        }

        void printInfo() {
            cout << endl;
            cout << "Number of clock cycles: " << noOfClockCycles << endl;
            cout << "Number of instructions executed: " << noOfInstructionsExecuted << endl;
            float instructionsPerCycle;
            if(noOfClockCycles == 0) {
                instructionsPerCycle = 0;
            }
            else {
                instructionsPerCycle = (float) noOfInstructionsExecuted / (float) noOfClockCycles;
            }
            cout << "Instruction per cycle: " << instructionsPerCycle << endl;
            cout << endl;
            cout << "PC: " << pc << endl;
            registerFile.printRegisters();
            cout << endl;
        }
};

int main(int argc, char *argv[]) {
    cout << "\n";

    //If command line arguments are incorrect then stop program
    if(argc != 2) {
        cout << "Machine code file not valid.\n";
        return 1;
    }

    //get the instructions from the machine code file
    string inputFileName(argv[1]);
    Instructions instructions(inputFileName);

    //create processor object and start processing
    if(instructions.getNumOfInstructions() != -1 && instructions.getInstructions() != NULL) {
        Processor processor(instructions);
        processor.start();
    }
    cout << "\n";
    return 0;
}

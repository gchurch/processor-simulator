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
#include "alu_reservation_station.h"
#include "branch_unit.h"
#include "branch_unit_reservation_station.h"
#include "store_buffer.h"
#include "load_buffer.h"
#include "load_store_unit.h"
#include "load_store_unit_reservation_station.h"
#include "decode_unit.h"
#include "fetch_unit.h"

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
    ALUReservationStation aluReservationStation;
    BranchUnit branchUnit;
    BranchUnitReservationStation branchUnitReservationStation;
    LoadStoreUnit loadStoreUnit;
    LoadStoreUnitReservationStation loadStoreUnitReservationStation;

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
            decodeUnit(&registerFile, &reorderBuffer, &aluReservationStation, &branchUnitReservationStation, &loadStoreUnitReservationStation, &decodeUnitBlockingFlag),
            alu(&registerFile, &reorderBuffer),
            aluReservationStation(&registerFile, &alu),
            branchUnit(&reorderBuffer),
            branchUnitReservationStation(&registerFile, &branchUnit),
            loadStoreUnit(&memory, &reorderBuffer, &loadStoreUnitBlockingFlag),
            loadStoreUnitReservationStation(&registerFile, &loadStoreUnit)
        {}


        void start() {

            cout << "Keep pressing ENTER to step through the program\n";

            printInfo();

            //step through the program
        	while(runningFlag) {

                //hold up the program at each clock cycle
                char str[3];
                fgets(str, 2, stdin);

                //if the pipeline is not being blocked
                if(!decodeUnitBlockingFlag && !loadStoreUnitBlockingFlag) {
                    //propogate values through pipeline
                    pipe();
                    //fetch the next instruction
                    fetch();
                }
                /*
                else {
                    if(decodeUnitBlockingFlag) {
                        printf("DECODE UNIT BLOCKING\n");
                    }
                    if(loadStoreUnitBlockingFlag) {
                        printf("LOAD STORE UNIT BLOCKING\n");
                    }
                }
                */

                //decode the instruction
                decode();

                //dispatch instructions from the reservation stations
                dispatch();

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
            //printf("EXECUTED DECODE\n");
        }

        void dispatch() {
            aluReservationStation.execute();
            branchUnitReservationStation.execute();
            //printf("EXECUTED DISPATCH\n");
            aluReservationStation.pipe();
            branchUnitReservationStation.pipe();
            //printf("EXECUTED DISPATCH PIPE\n");
        }

        void execute() {
            alu.execute();
            branchUnit.execute();
            loadStoreUnit.execute();
            //printf("EXECUTED EXECUTE\n");
        }

        void writeResult() {
            alu.writeResult();
            loadStoreUnit.writeResult();
            branchUnit.writeResult();
            //printf("EXECUTED WRITEBACK\n");
        }

        void commit() {
            reorderBuffer.checkTailForStore();
            reorderBuffer.retire();
            //printf("EXECUTED COMMIT\n");
        }

        void flushPipeline() {
            //flush fetch unit
            fetchUnit.flush();
            //flush decode unit
            decodeUnit.flush();
            //flush reservation stations
            aluReservationStation.flush();
            branchUnitReservationStation.flush();
            loadStoreUnitReservationStation.flush();
            //flush execution units
            alu.flush();
            branchUnit.flush();
            loadStoreUnit.flush();
            //flush reorder buffer
            reorderBuffer.flush();
            //reset the register file scoreboard
            registerFile.resetScoreBoard();
            //reset the flush flag
            flushFlag = 0;

            printf("FLUSHING PIPELINE!\n");
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

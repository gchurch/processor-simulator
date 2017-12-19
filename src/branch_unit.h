#ifndef _BRANCH_UNIT_INCLUDED_
#define _BRANCH_UNIT_INCLUDED_

class BranchUnit {
	
    ReorderBuffer* reorderBuffer;

    //decoded instruction
    int opcode;
    int operands[3];

    //set pc program to jump to a position in the program
    int* pc;

    //set to one to send message to processor to flush the pipeline
    int* flushFlag;
    int* runningFlag;

    //position in the reorder buffer
    int reorderBufferIndex;

    //if the branch condition is met or not
    int successful;

    public:
        BranchUnit(ReorderBuffer* reorderBuffer) : 
            reorderBuffer(reorderBuffer),
            pc(pc),
            flushFlag(flushFlag),
            runningFlag(runningFlag),
            opcode(0),
            reorderBufferIndex(-1),
            successful(0)
        {
            for(int i = 0; i < 3; i++) {
                operands[i] = 0;
            }
        }

        void execute() {
            if(opcode != 0) {
                //tell reorder buffer that we are executing the instruction
                reorderBuffer->executingEntry(reorderBufferIndex);
                //execute the instruction
                switch(opcode) {
                    case BEQ:
                        if(operands[0] == operands[1]) {
                            successful = 1;
                            //*pc = operands[2];
                            //*flushFlag = 1;
                        }
                        break;
                    case BNE:
                        if(operands[0] != operands[1]) {
                            successful = 1;
                            //*pc = operands[2];
                            //*flushFlag = 1;
                        }
                        break;
                    case J:
                        //*pc = operands[0];
                        //*flushFlag = 1;
                         break;
                    case HALT:
                        successful = 1;
                        //tell the processor that the program had finished
                        //*runningFlag = 0;
                        break;
                } 

                //reset variables
                opcode = 0;
                for(int i = 0; i < 3; i++) {
                    operands[i] = 0;
                }
            }
        }

        void writeResult() {
            if(reorderBufferIndex != -1) {
                //tell the reorder buffer that we are finished executing the instruction
                reorderBuffer->finishedEntry(reorderBufferIndex, successful);
            }
            reorderBufferIndex = -1;
            successful = 0;
        }

        void setOpcode(int x) {
            opcode = x;
        }

        void setOperands(int x[3]) {
            for(int i = 0; i < 3; i++) {
                operands[i] = x[i];
            }
        }

        void flush() {
            opcode = 0;
            for(int i = 0; i < 3; i++) {
                operands[i] = 0;
            }
            reorderBufferIndex = -1;
        }

        void setReorderBufferIndex(int i) {
            reorderBufferIndex = i;
        }
};

#endif
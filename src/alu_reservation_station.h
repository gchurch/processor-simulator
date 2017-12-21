#ifndef ALU_RESERVATION_STATION_H
#define ALU_RESERVATION_STATION_H

class ALUReservationStation {
    RegisterFile* registerFile;
    ALU* alu;

	int size;
	Instruction* instructions;
	int* reorderBufferIndexes;

	int opcode;
	int* operands;
	int reorderBufferIndex;

public:
	ALUReservationStation(RegisterFile* registerFile, ALU* alu) : 
	registerFile(registerFile),
	alu(alu),
	size(10),
	opcode(0),
	reorderBufferIndex(-1)
	{
		//allocate memory to the buffer
		instructions = new Instruction[size];
		//inialise all instructions to NOOPs
		for(int i = 0; i < size; i++) {
			instructions[i] = (Instruction) {0,0,0,0};
		}
		//allocate memory to the reorder buffer indexes array
		reorderBufferIndexes = new int[size];
		//initialise all reorder buffer indexes to -1
		for(int i = 0; i < size; i++) {
			reorderBufferIndexes[i] = -1;
		}
		//allocate memory for the operands array
		operands = new int[3];
		//initialising operands
		for(int i = 0; i < 3; i++) {
			operands[i] = 0;
		}
	}

	void execute() {
        //try and find an instruction that can be dispatched
        for(int i = 0; i < size; i++) {
		    if(instructions[i].opcode != NOOP) {
			    if(readyToDispatch(instructions[i])) {
                    dispatch(instructions[i]);
                    reorderBufferIndex = reorderBufferIndexes[i];

                    //printf("DISPATCHING INSTRUCTION: ");
                    //Instructions::printInstruction(instructions[i]);
                    //printf("WITH REORDER BUFFER INDEX: %d\n", reorderBufferIndex);

                    //clear the dispatched instruction from the reservation station
                    instructions[i] = (Instruction) {0,0,0,0};
                }
			}
		}
	}

	void addInstruction(Instruction instruction, int rbi) {
		//printf("ADDED INSTRUCTION: ");
		//Instructions::printInstruction(instruction);
        int index = findFreePosition();
		instructions[index] = instruction;
		reorderBufferIndexes[index] = rbi;
	}

    int findFreePosition() {
        for(int i = 0; i < size; i++) {
            if(instructions[i].opcode == NOOP) {
                return i;
            }
        }
        return -1;
    }

    void pipe() {
        if(opcode != 0) {
            //send the decoded instruction to the execution unit
            alu->setOpcode(opcode);
            alu->setOperands(operands);
            //Send the reorder buffer index to the alu
            alu->setReorderBufferIndex(reorderBufferIndex);
        
            //reset the outputs
            opcode = 0;
            for(int i = 0; i < 3; i++) {
                operands[i] = 0;
            }
            reorderBufferIndex = -1;
        }
    }

    void flush() {
        for(int i = 0; i < size; i++) {
            instructions[i] = (Instruction) {0,0,0,0};
            reorderBufferIndexes[i] = -1;
        }
        opcode = 0;
        for(int i = 0; i < 3; i++) {
            operands[i] = 0;
        }
        reorderBufferIndex = -1;
    }

    void print() {
        printf("ALU RESERVATION STATION:\n");
        for(int i = 0; i < size; i++) {
            if(instructions[i].opcode != NOOP) {
                Instructions::printInstruction(instructions[i]);
            }
        }
    }

private:

	int readyToDispatch(Instruction instruction) {
        //check that the source registers are ready to use
		switch(instruction.opcode) {
			case ADD:
			case AND:
			case MULT:
			case OR:
			case SUB:
			    if(registerFile->getScoreBoardValue(instruction.operands[1]) && registerFile->getScoreBoardValue(instruction.operands[2])) {
			    	return 1;
			    }
			    break;
			case ADDI:
			    if(registerFile->getScoreBoardValue(instruction.operands[1])) {
			    	return 1;
			    }
			    break;
		}
		return 0;
	}

    //dispatch bound fetch
    void dispatch(Instruction instruction) {
        //getting the opcode and incomplete operands from the instruction
        opcode = instruction.opcode;
        operands = new int[3];
        for(int i = 0; i < 3; i++) {
        	operands[i] = instruction.operands[i];
        }
        //temp variables
        int registerNum;
        int val;
        //fetching the operands for the instruction
        switch(opcode) {
            case ADD:
            case AND:
            case MULT:
            case OR:
            case SUB:
                for(int i = 1; i < 3; i++) {
                    registerNum = operands[i];
                    val = registerFile->getRegisterValue(registerNum);
                    operands[i] = val;
                }
                break;
            case ADDI:
                registerNum = operands[1];
                val = registerFile->getRegisterValue(registerNum);
                operands[1] = val;
                break;
        }
        //Setting the scoreBoard values of the destination register to 0
        registerFile->setScoreBoardValue(operands[0],0);
    }
};

#endif
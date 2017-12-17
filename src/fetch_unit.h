class FetchUnit {
    Instruction* instructions;
    int noOfInstructions;
    int* pc;
    DecodeUnit* decodeUnit;
    Instruction currentInstruction;

    public:
        FetchUnit(Instruction* instructions, int noOfInstructions, int* pc, DecodeUnit* decodeUnit) :
        instructions(instructions),
        noOfInstructions(noOfInstructions),
        pc(pc),
        decodeUnit(decodeUnit),
        currentInstruction((Instruction) {0,0,0,0})
    {}

    void execute() {
        if(*pc <= noOfInstructions) {
            //fetch the next instruction (-1 so that pc of 1 refers to the first instruction on line 1)
    	    currentInstruction = instructions[*pc - 1];
        }
        else {
            //next instruction is noop if pc exceeds number of instructions
            currentInstruction = (Instruction) {0,0,0,0};
        }
        //increment the program counter
        (*pc)++;
    }

    void pipe() {
        //put the fetched instruction into the instruction register
        decodeUnit->setNextInstruction(currentInstruction);
    }

    void flush() {
        currentInstruction = (Instruction) {0,0,0,0};
    }
};

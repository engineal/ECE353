//Array of registers
struct Register registers[32];
//Instruction memory
struct Instruction instructionMem[64];
//Data memory
int dataMem[64];

// IF
struct LatchA *instructionFetch(int pc) {
    struct LatchA *result = malloc(sizeof(struct LatchA));
    result->instruction = instructionMem[pc];
    return result;
}

// ID
struct LatchB *instructionDecode(struct LatchA *state){
    struct LatchB *result = malloc(sizeof(struct LatchB));
	// Check that registers are unflagged
	//flag == false, register not safe
	if (registers[state->instruction.rs].flag == false ||
        registers[state->instruction.rt].flag == false ) {
		
		// send NOP add $0, $s0, $s0
		result->opcode = add; 
		result->rd = 0; 
		result->reg1 = 0;
		result->reg2 = 0;
	}
	//flag == true, registers safe
	else{
		result->opcode = state->instruction.opcode; 
		result->rd = state->instruction.rd; //pass reg #
		result->reg1 = registers[state->instruction.rs].value; //pass reg value
		result->reg2 = registers[state->instruction.rt].value; //pass reg value
	}
    // Set LatchA ready value to…..? 
    result->ready = true;
    
    return result;
}

// EXE
struct LatchC *execute(struct LatchB *state) {
    int aluResult = 0;
    switch (state->opcode) {
    case add:
        aluResult = state->reg1 + state->reg2;
        break;
    case sub:
        aluResult = state->reg1 - state->reg2;
        break;
    case mul:
        aluResult = state->reg1 * state->reg2;
        break;
    case sw:
    case lw:
        aluResult = state->reg1 + state->immediate;
        break;
    }
    
    struct LatchC *result = malloc(sizeof(struct LatchC));
    result->opcode = state->opcode;
    result->rd = state->rd;
    result->reg2 = state->reg2;
    result->result = aluResult;
    result->ready = true;
    
    return result;
}

// MEM
struct LatchD *memory(struct LatchC *state) {
    int memResult = 0;
    switch (state->opcode) {
    case sw:
        dataMem[state->result] = state->reg2;
        break;
    case lw:
        memResult = dataMem[state->result];
        break;
    }
    
    struct LatchD *result = malloc(sizeof(struct LatchD));
    result->opcode = state->opcode;
    result->rd = state->rd;
    result->result = memResult;
    result->ready = true;
    
    return result;
}

// WB
void writeBack(struct LatchD *state) {
    switch (state->opcode) {
    case add:
    case sub:
    case addi:
    case lw:
    case mul:
        registers[state->rd].value = state->result;
        break;
    }
}

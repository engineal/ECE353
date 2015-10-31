//Array of registers
struct Register registers[32];
//Instruction memory
struct Instruction instructionMem[1024];
//Data memory
int dataMem[1024];

// IF
bool instructionFetch(int pc, struct LatchA *result) {
    result->instruction = instructionMem[pc];
    return true;
}

// ID
bool instructionDecode(struct LatchA *state, struct LatchB *result){
	// Check that registers are unflagged
	// flag == false, register not safe
	if (registers[state->instruction.rs].flag == false ||
        registers[state->instruction.rt].flag == false ) {
		
		// send NOP add $0, $s0, $s0
		result->opcode = add; 
		result->rd = 0; 
		result->reg1 = 0;
		result->reg2 = 0;
        
        return false;
	}
	//flag == true, registers safe
	else{
		result->opcode = state->instruction.opcode; 
		result->rd = state->instruction.rd; //pass reg #
		result->reg1 = registers[state->instruction.rs].value; //pass reg value
		result->reg2 = registers[state->instruction.rt].value; //pass reg value
        
        return true;
	}
}

// EXE
bool execute(struct LatchB *state, struct LatchC *result, int multiplyCycles, int otherCycles) {
    if (state->cycles < 0) {
        if (state->opcode == mul) {
            state->cycles = multiplyCycles;
        } else {
            state->cycles = otherCycles;
        }
    }
    
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
    
    if (state->cycles == 0) {
        state->cycles--;
        result->opcode = state->opcode;
        result->rd = state->rd;
        result->reg2 = state->reg2;
        result->result = aluResult;
        return true;
    } else {
        state->cycles--;
        return false;
    }
}

// MEM
bool memory(struct LatchC *state, struct LatchD *result, int accessCycles) {
    if (state->cycles < 0) {
        state->cycles = accessCycles;
    }
    int memResult = 0;
    switch (state->opcode) {
    case sw:
        dataMem[state->result] = state->reg2;
        break;
    case lw:
        memResult = dataMem[state->result];
        break;
    }
    
    if (state->cycles == 0) {
        state->cycles--;
        result->opcode = state->opcode;
        result->rd = state->rd;
        result->result = memResult;
        return true;
    } else {
        state->cycles--;
        return false;
    }
}

// WB
bool writeBack(struct LatchD *state) {
    if (state->rd != 0) {
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
    
    return true;
}

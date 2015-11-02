//Array of registers
struct Register registers[32];
//Instruction memory
struct Instruction instructionMem[1024];
//Data memory
int dataMem[1024];

// IF
bool instructionFetch(int pc, struct LatchA *result) {
    if (instructionMem[pc].opcode == haltSimulation) {
        result->instruction.opcode = haltSimulation;
        result->instruction.rt = 0;
        result->instruction.rs = 0;
        result->instruction.rd = 0;
        result->instruction.immediate = 0;
        return false;
    }
    
    if (result->cycles == 0) {
        if (instructionMem[pc].opcode == beq) {
            result->cycles = 3;
        } else {
            result->cycles = 1;
        }
    }
    result->cycles--;
    if (result->cycles == 0) {
        result->instruction = instructionMem[pc];
        return true;
    } else if(instructionMem[pc].opcode == beq) {
        result->instruction = instructionMem[pc];
        return false;
    } else {
        result->instruction.opcode = add;
        result->instruction.rt = 0;
        result->instruction.rs = 0;
        result->instruction.rd = 0;
        result->instruction.immediate = 0;
        return false;
    }
}

// ID
bool instructionDecode(struct LatchA *state, struct LatchB *result) {
    if (state->instruction.opcode == haltSimulation) {
        result->opcode = haltSimulation;
        result->reg1 = 0;
        result->reg2 = 0;
        result->regResult = 0;
        result->immediate = 0;
        return false;
    }
    
    bool readRT;
    switch (state->instruction.opcode) {
        case add:
        case sub:
        case mul:
        case beq:
        case sw:
            readRT = true;
            break; 
        case addi:
        case lw:
            readRT = false;
            break;
    }
    // Check that registers are unflagged
    // flag == false, register not safe
    if (registers[state->instruction.rs].flag && (!readRT || registers[state->instruction.rt].flag)) {
        //flag == true, registers safe
        result->opcode = state->instruction.opcode; 
        result->reg1 = registers[state->instruction.rs].value; //pass reg value
        if (readRT) {
            result->reg2 = registers[state->instruction.rt].value; //pass reg value
        } else {
            result->reg2 = 0;
        }
        switch (state->instruction.opcode) {
            case add:
            case sub:
            case mul:
                result->regResult = state->instruction.rd;
                break; 
            case addi:
            case lw:
                result->regResult = state->instruction.rt;
                break;
            default:
                result->regResult = 0;
                break;
        }
        
        //set write register flags to false (not safe)
        if (result->regResult != 0) {
            registers[result->regResult].flag = false;
        }
        result->immediate = state->instruction.immediate;
        
        return true;
    } else {
        // send NOP add $0, $s0, $s0
        result->opcode = add;
        result->reg1 = 0;
        result->reg2 = 0;
        result->regResult = 0;
        result->immediate = 0;
        
        return false;
    }
}

// EXE
bool execute(struct LatchB *state, struct LatchC *result, long *pc, int multiplyCycles, int otherCycles) {
    if (state->opcode == haltSimulation) {
        result->opcode = haltSimulation;
        result->reg2 = 0;
        result->regResult = 0;
        result->result = 0;
        return false;
    }
    
    if (state->cycles == 0) {
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
    case beq:
        aluResult = state->reg1 - state->reg2;
        break;
    case mul:
        aluResult = state->reg1 * state->reg2;
        break;
    case addi:
    case sw:
    case lw:
        aluResult = state->reg1 + state->immediate;
        break;
    }
    
    state->cycles--;
    if (state->cycles == 0) {
        if (state->opcode == beq && aluResult == 0) {
            (*pc) += state->immediate;
        }
        result->opcode = state->opcode;
        result->reg2 = state->reg2;
        result->regResult = state->regResult;
        result->result = aluResult;
        return true;
    } else {
        return false;
    }
}

// MEM
bool memory(struct LatchC *state, struct LatchD *result, int accessCycles) {
    if (state->opcode == haltSimulation) {
        result->opcode = haltSimulation;
        result->regResult = 0;
        result->result = 0;
        return false;
    }
    
    if (state->cycles == 0) {
        switch (state->opcode) {
        case sw:
        case lw:
            state->cycles = accessCycles;
            break;
        default:
            state->cycles = 1;
            break;
        }
    }
    int memResult = 0;
    switch (state->opcode) {
    case sw:
        dataMem[state->result] = state->reg2;
        break;
    case lw:
        memResult = dataMem[state->result];
        break;
    default:
        memResult = state->result;
        break;
    }
    
    state->cycles--;
    if (state->cycles == 0) {
        result->opcode = state->opcode;
        result->regResult = state->regResult;
        result->result = memResult;
        return true;
    } else {
        return false;
    }
}

// WB
bool writeBack(struct LatchD *state) {
    if (state->opcode == haltSimulation){
        return false;
    }
    
    switch (state->opcode) {
    case add:
    case sub:
    case addi:
    case lw:
    case mul:
        if (state->regResult != 0) {
            registers[state->regResult].value = state->result;
            registers[state->regResult].flag = true;
        }
        break;
    default:
        break;
    }
    
    return true;
}

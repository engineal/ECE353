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
    }
    
    struct LatchC *result = malloc(sizeof(struct LatchC));
    result->opcode = state->opcode;
    result->rd = state->rd;
    result->result = aluResult;
    result->ready = true;
    
    return result;
}

int *dataMem = malloc(sizeof(int) * 64);

struct LatchD *memory(struct LatchC *state) {
    int memResult = 0;
    switch (state->opcode) {
    case sw:
        dataMem[state->result] = 0;
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
}
#include "structs.c"

bool verifyRegister(int reg);
bool verifyAddress(int address);

bool verifyInstruction(struct Instruction *inst) {
    bool result = true;
    result &= verifyRegister(inst->rs);
    result &= verifyRegister(inst->rt);
    result &= verifyRegister(inst->rd);
    
    switch (inst->opcode) {
    case sw:
    case lw:
        result &= verifyAddress(inst->address);
        break;
    }
    
    return result;
}

bool verifyRegister(int reg) {
    return reg >= 0 && reg < 32;
}

bool verifyAddress(int address) {
    return address >= 0 && address < 64;
}
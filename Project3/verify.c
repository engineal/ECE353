boolean verifyInstruction(struct Instruction inst) {
    boolean result = true;
    result &&= verifyRegister(inst.rs);
    result &&= verifyRegister(inst.rt);
    result &&= verifyRegister(inst.rd);
    
    switch (state->opcode) {
    case sw:
    case lw:
        result &&= verifyAddress(inst.address);
        break;
    }
    
    return result;
}

boolean verifyRegister(int reg) {
    return reg > 0 && reg < 32;
}

boolean verifyAddress(int address) {
    return address > 0 && address < 64;
}
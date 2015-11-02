#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "sim-mips.c"

bool registerStringtoIntTest(void);
bool stringToInstructionTest(void);
bool verifyOpcodeTest(void);
bool verifyRegisterTest(void);
bool instructionFetchTest(void);
bool instructionDecodeTest(void);
bool executeTest(void);
bool memoryTest(void);
bool writeBackTest(void);

int main(int argc, char *argv[]) {
    bool result = true;
    
    result &= registerStringtoIntTest();
    result &= stringToInstructionTest();
    result &= verifyOpcodeTest();
    result &= verifyRegisterTest();
    result &= executeTest();
    result &= writeBackTest();
    
    if (result) {
        printf("Passed!");
    } else {
        printf("Failed!");
    }
    return result;
}

bool registerStringtoIntTest(void) {
    printf("registerStringtoInt() Test\n");
	char *input[] = {"Hello", "$zero", "$0", "0", "$AT", "$t8", "$ra", "$gP", "29"};
    int result[] = {-1, 0, 0, 0, 1, 24, 31, 28, 29};
    bool pass = true;
    
    int i;
    for (i = 0; i < 9; i++) {
        int r = registerStringtoInt(input[i]);
        printf("input: %s, expected result: %d, result: %d\n", input[i], result[i], r);
        pass &= (result[i] == r);
    }
    
    printf("\t%s\n\n", pass ? "true" : "false");
    return pass;
}

bool stringToInstructionTest(void) {
    printf("stringToInstruction() Test\n");
    
    char *input[7];
    struct Instruction result[7];
    //t0 =8, s0 = 16, s1= 17
    input[0] = "add $s0 $s1 $t0";
    result[0].opcode = add;
    result[0].rs = 17;
    result[0].rt = 8;
    result[0].rd = 16;
    result[0].immediate = 0;
    
    input[1] = "sub $s0 $s1 $t0";
    result[1].opcode = sub;
    result[1].rs = 17;
    result[1].rt = 8;
    result[1].rd = 16;
    result[1].immediate = 0;
    
    input[2] = "mul $s0 $s1 $t0";
    result[2].opcode = mul;
    result[2].rs = 17;
    result[2].rt = 8;
    result[2].rd = 16;
    result[2].immediate = 0;

    input[3] = "addi $s0 $s1 156";
    result[3].opcode = addi;
    result[3].rs = 17;
    result[3].rt = 8;
    result[3].rd = 0;
    result[3].immediate = 156;

    input[4] = "lw $t0 156($s1)";
    result[4].opcode = lw;
    result[4].rs = 17;
    result[4].rt = 8;
    result[4].rd = 0;
    result[4].immediate = 156;

    input[5] = "sw $t0 156($s1)";
    result[5].opcode = sw;
    result[5].rs = 17;
    result[5].rt = 8;
    result[5].rd = 0;
    result[5].immediate = 156;

    input[6] = "beq $s0 $s1 156";
    result[6].opcode = beq;
    result[6].rs = 17;
    result[6].rt = 8;
    result[6].rd = 0;
    result[6].immediate = 156;

    bool pass = true;
    
    int i;
    for (i = 0; i < 1; i++) {
        struct Instruction *inst = stringToInstruction(input[i]);
        pass &= result[i].opcode == inst->opcode;
        pass &= result[i].rs == inst->rs;
        pass &= result[i].rt == inst->rt;
        pass &= result[i].rd == inst->rd;
        pass &= result[i].immediate == inst->immediate;
    }
    
    printf("\t%s\n\n", pass ? "true" : "false");
    return pass;
}

bool verifyOpcodeTest(void) {
    printf("verifyOpcode() Test\n");
	Opcode input[] = {-1, add, sub, addi, mul, lw, sw, beq, haltSimulation, 8};
    bool result[] = {false, true, true, true, true, true, true, true, false};
    bool pass = true;
    
    int i;
    for (i = 0; i < 7; i++) {
        bool r = verifyOpcode(input[i]);
        printf("input: %d, expected result: %s, result: %s\n", input[i], result[i] ? "true" : "false", r ? "true" : "false");
        pass &= (result[i] == r);
    }
    
    printf("\t%s\n\n", pass ? "true" : "false");
    return pass;
}

bool verifyRegisterTest(void) {
    printf("verifyRegister() Test\n");
	int input[] = {-1, 0, 1, 20, 30, 31, 32};
    bool result[] = {false, true, true, true, true, true, false};
    bool pass = true;
    
    int i;
    for (i = 0; i < 7; i++) {
        bool r = verifyRegister(input[i]);
        printf("input: %d, expected result: %s, result: %s\n", input[i], result[i] ? "true" : "false", r ? "true" : "false");
        pass &= (result[i] == r);
    }
    
    printf("\t%s\n\n", pass ? "true" : "false");
    return pass;
}

bool executeTest(void) {
    printf("execute() Test\n");
    int i;
    for (i = 0; i < 32; i++){
        registers[i].value = 0;
        registers[i].flag = true;
    }
    
    int pc = 0;
    
    struct LatchB *stateB = malloc(sizeof(struct LatchB));
    stateB->opcode = add;
    stateB->reg1 = 5;
    stateB->reg2 = 9;
    stateB->regResult = 2;
    stateB->immediate = 0;
    stateB->cycles = 0;

    struct LatchC *stateC = malloc(sizeof(struct LatchC));
    
    execute(stateB, stateC, 1, 1);
    
    bool pass = true;
    pass &= (stateC->result == 14);
    
    printf("\t%s\n\n", pass ? "true" : "false");
    return pass;
}

bool writeBackTest(void) {
    printf("writeBack() Test\n");
    int i;
    for (i = 0; i < 32; i++){
        registers[i].value = 0;
        registers[i].flag = true;
    }
    
    struct LatchD *stateD = malloc(sizeof(struct LatchD));
    stateD->opcode = add;
    stateD->regResult = 5;
    stateD->result = 5;
    
    writeBack(stateD);
    
    bool pass = true;
    pass &= (registers[5].value == 5);
    
    printf("\t%s\n\n", pass ? "true" : "false");
    return pass;
}
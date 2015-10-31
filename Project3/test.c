#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>


#include "sim-mips.c"

bool registerStringtoIntTest(void);
bool clockTickTest(void);
bool stringToInstructionTest(void);

int main(int argc, char *argv[]) {
    bool result = true;
    
    result &= registerStringtoIntTest();
    result &= clockTickTest();
    result &= stringToInstructionTest();
    
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
    
    return pass;
}

bool clockTickTest(void) {
    printf("clockTick() Test\n");
    
    int i;
    for (i = 0; i < 32; i++){
        registers[i].value = 0;
        registers[i].flag = true;
    }
    
    long pc = 0;
    struct LatchA *stateA = malloc(sizeof(struct LatchA));
    stateA->instruction.opcode = add;
    stateA->instruction.rs = 0; 
    stateA->instruction.rt = 0;
    stateA->instruction.rd = 0;
    
    struct LatchB *stateB = malloc(sizeof(struct LatchB));
    stateB->opcode = add;
    stateB->reg1 = 0;
    stateB->reg2 = 0;
    stateB->regResult = 0;
    
    struct LatchC *stateC = malloc(sizeof(struct LatchC));
    stateC->opcode = add;
    stateC->reg2 = 0;
    stateC->result = 0;
    stateC->regResult = 0;
    
    struct LatchD *stateD = malloc(sizeof(struct LatchD));
    stateD->opcode = add;
    stateD->result = 5;
    stateD->regResult = 1;
    
    for (i = 0; i < 32; i++){
        //printf("%2d: %d\n", i, registers[i].value);
    }
    
    bool pass = true;
    
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

    input[4] = "lw $t0 156";
    result[4].opcode = lw;
    result[4].rs = 0;
    result[4].rt = 8;
    result[4].rd = 0;
    result[4].immediate = 156;

    input[5] = "sw $t0 156";
    result[5].opcode = sw;
    result[5].rs = 0;
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
    
    return pass;
}
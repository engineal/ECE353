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

int main(int argc, char *argv[]) {
    bool result = true;
    
    result &= registerStringtoIntTest();
    result &= clockTickTest();
    
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
    stateB->rd = 0;
    stateB->reg1 = 0;
    stateB->reg2 = 0;
    
    struct LatchC *stateC = malloc(sizeof(struct LatchC));
    stateC->opcode = add;
    stateC->rd = 0;
    stateC->reg2 = 0;
    stateC->result = 0;
    
    struct LatchD *stateD = malloc(sizeof(struct LatchD));
    stateD->opcode = add;
    stateD->rd = 1;
    stateD->result = 5;
    
    for (i = 0; i < 32; i++){
        printf("%2d: %d\n", i, registers[i].value);
    }
    
    bool pass = true;
    
    return pass;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>


#include "sim-mips.c"

bool RegisterStringtoIntTest(void);
bool ArraytoInstructionTest(void);

int main(int argc, char *argv[]) {
    bool result = true;
    
    printf("RegisterStringtoInt() Test");
    result &= RegisterStringtoIntTest();
    
    if (result) {
        printf("Passed!");
    } else {
        printf("Failed!");
    }
    return result;
}

bool RegisterStringtoIntTest(void) {
	char *input[] = {"Hello", "$AT"};
    int result[] = {-1, 1};
    bool pass = true;
    
    int i;
    for (i = 0; i < 1; i++) {
        pass &= (result[i] == RegisterStringtoInt(input[i]));
    }
    
    return pass;
}

bool ArraytoInstructionTest(void) {
    char *input[7];
    struct Instruction result[7];

    input[0] = "add $s0 $s1 $t0";
    result[0].opcode = add;

    input[1] = "sub $s0 $s1 $t0";
    result[1].opcode = sub;
    
    input[2] = "mul $s0 $s1 $t0";
    result[2].opcode = mul;

    srand(time(NULL));
    int r = rand();

    input[3] = "addi $s0 $s1 r";
    result[3].opcode = addi;

    input[4] = "lw $s0 $s1 r";
    result[4].opcode = lw;

    input[5] = "sw $s0 $s1 r";
    result[5].opcode = sw;

    input[6] = "beq $s0 $s1 r";
    result[6].opcode = beq;

    bool pass = true;
    
    int i;
    for (i = 0; i < 1; i++) {
        pass &= (result[i] == arraytoInstruction(input[i]));
    }
    
    return pass;
}
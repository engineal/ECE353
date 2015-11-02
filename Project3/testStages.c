#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#include "structs.c"
#include "stages.c"

bool instructionFetchTest(void);
bool instructionDecodeTest(void);
bool executeTest(void);
bool memoryTest(void);
bool writeBackTest(void);

int main(int argc, char *argv[]) {
    bool result = true;
    
    result &= executeTest();
    result &= writeBackTest();
    
    if (result) {
        printf("Passed!\n");
    } else {
        printf("Failed!\n");
    }
    return result;
}

bool executeTest(void) {
    printf("execute() Test\n");
    int i;
    for (i = 0; i < 32; i++){
        registers[i].value = 0;
        registers[i].flag = true;
    }
    
    int pc = 0;
    
    struct LatchD *stateC = malloc(sizeof(struct LatchC));
    stateC->opcode = add;
    stateC->regResult = 5;
    stateC->result = 5;
    
    struct LatchD *stateD = malloc(sizeof(struct LatchD));
    stateD->opcode = add;
    stateD->regResult = 5;
    stateD->result = 5;
    
    execute(stateC, stateD);
    
    bool pass = true;
    pass &= (registers[5].value == 5);
    
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
    
    return pass;
}
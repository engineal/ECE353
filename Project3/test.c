#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "sim-mips.c"

bool RegisterStringtoIntTest(void);

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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#include "verify.c"

bool verifyRegisterTest(void);
bool verifyAddressTest(void);

int main(int argc, char *argv[]) {
    bool result = true;
    
    result &= verifyRegisterTest();
    result &= verifyAddressTest();
    
    if (result) {
        printf("Passed!\n");
    } else {
        printf("Failed!\n");
    }
    return result;
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
    
    return pass;
}

bool verifyAddressTest(void) {
    printf("verifyAddress() Test\n");
	int input[] = {-1, 0, 1, 30, 62, 63, 64};
    bool result[] = {false, true, true, true, true, true, false};
    bool pass = true;
    
    int i;
    for (i = 0; i < 7; i++) {
        bool r = verifyAddress(input[i]);
        printf("input: %d, expected result: %s, result: %s\n", input[i], result[i] ? "true" : "false", r ? "true" : "false");
        pass &= (result[i] == r);
    }
    
    return pass;
}
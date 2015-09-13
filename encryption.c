// Reads the input file in plainTextFile.txt,
// character by character and then ex-ors them
// with the previous character.  To start things off,
// we ex-or the initial character with a given input.
// The output is then written to an output file.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define MAX_LENGTH 256

void swap (char *, char *);
char generateKeyByte (char []);

int main(void) {
    FILE *plainTextFile;
    FILE *keyStreamFile;
    FILE *encryptedTextFile;
    char inputChar;
    char outputChar;
    char keyChar; 
    int kLength = 0;
    char key[MAX_LENGTH];
    char S[MAX_LENGTH];
    char T[MAX_LENGTH];

    plainTextFile = fopen("./plainText.txt", "r");
    keyStreamFile = fopen("./keyFile.txt", "r"); 
    encryptedTextFile = fopen("./encryptedText.txt", "w");

    assert(plainTextFile != NULL);
    assert(keyStreamFile != NULL); 
    assert(encryptedTextFile != NULL);

    // fill in key
    while (((keyChar = fgetc(keyStreamFile)) != EOF) && kLength < MAX_LENGTH) {
        key[kLength] = keyChar;
        kLength++; 
    }

    //fill in S, T
    for (int i = 0; i < MAX_LENGTH; i++){
        S[i] = i; 
        T[i] = key[i % kLength];
    }

    //initial permutation
    int j =0; 
    for (int i =0; i< MAX_LENGTH; i++){
        j = (j+S[i]+T[i]) % MAX_LENGTH; 
        swap (&S[i], &S[j]);
    }

    //Part 3: for each character read, generate the next key stream elem. Xor key byte with read byte to form encrypted output. Write to output file.
    while ((inputChar = fgetc(plainTextFile)) != EOF) {
        keyChar = generateKeyByte(S);
        outputChar = inputChar ^ keyChar;
        fputc(outputChar, encryptedTextFile); 
    }

    fclose(plainTextFile);
    fclose(keyStreamFile);
    fclose(encryptedTextFile);
}//end of main

char generateKeyByte (char S[]){
    static int i = 0;
    static int j = 0;
    i = (i+1) % MAX_LENGTH; 
    j = (j+S[i]) % MAX_LENGTH; 
    swap (&S[i], &S[j]);
    int t = (S[i]+S[j]) % MAX_LENGTH; 
    return S[t];
}

void swap (char * a, char * b){
    char temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

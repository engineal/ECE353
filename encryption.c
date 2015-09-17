/*ECE 353 Lab 0: Encryption and Decryption
Sarah Mangels, Matteo Puzella, Aaron Lucia
Sept 13, 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define MAX_LENGTH 256

void swap (int *, int *);
int generateKeyByte (int []);

int main(void) {
    FILE *plainTextFile;
    FILE *keyStreamFile;
    FILE *encryptedTextFile;
    int inputChar;
    int outputChar;
    int keyChar;
    int kLength = 0;
    int key[MAX_LENGTH];
    int S[MAX_LENGTH];
    int T[MAX_LENGTH];

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
    int i;
    for (i = 0; i < MAX_LENGTH; i++){
        S[i] = i; 
        T[i] = key[i % kLength];
    }

    //initial permutation
    int j = 0;
    for (i = 0; i< MAX_LENGTH; i++){
        j = (j+S[i]+T[i]) % MAX_LENGTH; 

        assert(&S[i] != NULL); 
        swap (&S[i], &S[j]);
    }

    assert(sizeof(S)>=256); 
    assert(sizeof(T)>=256);

    //Part 3: for each character read, generate the next key stream elem. Xor key byte   with read  byte to form encrypted output. Write to output file.
    while ((inputChar = fgetc(plainTextFile)) != EOF) {
        keyChar = generateKeyByte(S);
        outputChar = inputChar ^ keyChar;
        fputc(outputChar, encryptedTextFile);
        if (outputChar == EOF) {
            
        }
    }

    fclose(plainTextFile);
    fclose(keyStreamFile);
    fclose(encryptedTextFile);
}//end of main

int generateKeyByte (int S[]){
    static int i = 0;
    static int j = 0;
    i = (i+1) % MAX_LENGTH; 
    j = (j+S[i]) % MAX_LENGTH; 
    swap (&S[i], &S[j]);
    int t = (S[i]+S[j]) % MAX_LENGTH; 
    return S[t];
}

void swap (int * a, int * b){
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

// Reads the input file in plainTextFile.txt,
// character by character and then ex-ors them
// with the previous character.  To start things off,
// we ex-or the initial character with a given input.
// The output is then written to an output file.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>


int main(void) {

    FILE *ifp;
    FILE *ofp;
    char inputChar;
    char outputChar;
    char prevChar;
    int i;


    ifp = fopen("./plainTextFile.txt", "r");
    ofp = fopen("./outputFile.txt", "w");

    assert(ifp != NULL);
    assert(ofp != NULL);

    prevChar = 'i';

     while ((inputChar = fgetc(ifp))!=EOF) {
       outputChar = inputChar ^ prevChar;
       fputc(outputChar, ofp);
       prevChar = outputChar;
     }
    fclose(ifp);
    fclose(ofp);

}
//ss

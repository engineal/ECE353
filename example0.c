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
	FILE *ksf; 
    FILE *ofp;
    char inputChar;
    char outputChar;å
	char next; 
	char keyChar; //next element of key stream
    int i;


    ifp = fopen("./plainText.txt", "r");
	ksf = fopen("./keyFile.txt", "r"); 
    ofp = fopen("./encryptedText.txt", "w");

    assert(ifp != NULL);
    assert(ofp != NULL);
	assert(ifp != NULL); 


//Count number of characters in key stream file 
int count = 0; 
while ((next = getc(ksf)) != EOF){
	count++; }
	





//Part 3: for each character read, generate the next key stream elem. Xor key byte with read byte to form encrypted output. Write to output file.
     while ((inputChar = fgetc(ifp))!=EOF) {
	//generate next element of key stream
       outputChar = inputChar ^ keyChar;
       fputc(outputChar, ofp);
      
     }


char generateKeyByte (char s[]){
}


void swap (char * a, char * b){
 char temp;
   temp = *a;
   *a = *b;
   *b = temp;
}


    fclose(ifp);
    fclose(ofp);
	fclose(ksf); 

}
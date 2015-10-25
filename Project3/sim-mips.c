// Author: Shikang Xu; ECE 353 TA

// List the full names of ALL group members at the top of your code.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "structs.c"
#include "stages.c"
//feel free to add here any additional library names you may need 
#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

struct Instruction *arrayToInstruction(char* tokens[]);

main (int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long mips_reg[REG_NUM];
	long pgm_c=0;//program counter
	long sim_cycle=0;//simulation cycle counter
	//define your own counter for the usage of each pipeline stage here
	
	int test_counter=0;
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for(i=1;i<argc;i++){
		printf("%s ",argv[i]);
	}
	printf("\n");
	if(argc==7){
		if(strcmp("-s",argv[1])==0){
			sim_mode=SINGLE;
		}
		else if(strcmp("-b",argv[1])==0){
			sim_mode=BATCH;
		}
		else{
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");
	}
	
	else{
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-sysle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
		printf("m,n,c stand for number of cycles needed by multiplication, other operation, and memory access, respectively\n");
		exit(0);
	}
	if(input==NULL){
		printf("Unable to open input or output file\n");
		exit(0);
	}
	if(output==NULL){
		printf("Cannot create output file\n");
		exit(0);
	}
	//initialize registers and program counter
	if(sim_mode==1){
		for (i=0;i<REG_NUM;i++){
			mips_reg[i]=0;
		}
	}
	
	//start your code from here

	//File read
	char *line = malloc(sizeof(char) * 136);
	while (fgets(line, 100, input) != NULL){
		char *tokens[6]; 
		char *token = strtok(line, " ,;)");
        tokens[0] = token;
		int i = 1;
		while (token != NULL) { //puts tokens into array, to be put into an strct Instruciton
			token = strtok(NULL, " ,;)");
			tokens[i++]=token;
		}

		struct Instruction *inst = arrayToInstruction(tokens);
	}
    
    struct LatchA *stateA = malloc(sizeof(struct LatchA));
    struct LatchB *stateB = malloc(sizeof(struct LatchB));
    struct LatchC *stateC = malloc(sizeof(struct LatchC));
    struct LatchD *stateD = malloc(sizeof(struct LatchD));

    int running;
    while (running) {
        writeBack(stateD);
        free(stateD);
        stateD = memory(stateC);
        free(stateC);
        stateC = execute(stateB);
        free(stateB);
        stateB = instructionDecode(stateA);
        free(stateA);
        stateA = instructionFetch(pgm_c);
        pgm_c++;
    }
    
} // end main


//takes in the tokens of an instruction as an array, returns a struct Instruction
struct Instruction *arrayToInstruction(char* tokens[]){
	struct Instruction *a = malloc(sizeof(struct Instruciton));

	switch(tokens[0]){
		case "add":
            a.opcode = add;
			a.rd = tokens[1]; 
			a.rs = tokens[2]; 
			a.rt = tokens[3];
		case "sub": 
            a.opcode = sub;
            a.rd = tokens[1]; 
			a.rs = tokens[2]; 
			a.rt = tokens[3];
		case "addi":
            a.opcode = addi;
		case "mul":
            a.opcode = mul;
            a.rd = tokens[1]; 
			a.rs = tokens[2]; 
			a.rt = tokens[3];
		case "lw":
            a.opcode = lw;
		case "sw":
            a.opcode = sw;
		case "beq":
            a.opcode = beq;
	}
    
    return a;
}

//takes in register string returns register number, to be used as index for registers[] array
int RegisterStringtoInt(char *s){
	s =tolower(s); 
	char* regNames = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]; 

	int reg; 
	if(sscanf(s, "%d", &reg)!=1){ //if its not a string version of an integer (if it is, the int was assigned to reg)
		for(int i = 0; i<32; i++){
			if (strcmp(regNames[i], s) == 0) { reg = i; } //they are the same
		}
	}

	//make sure regs is within 
	assert(reg<32); 
	assert(reg<=0); 

	return reg; 

	/* to do : what if the register in invalid? ie out of 0-31 or a word etc*/	
}
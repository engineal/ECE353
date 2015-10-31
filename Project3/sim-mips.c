// Authors: Aaron Lucia, Sarah Mangles, Matteo Puzella

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#include "structs.c"
#include "stages.c"
#include "verify.c"

#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

struct Instruction *stringToInstruction(char* line);
int registerStringtoInt(char *s);
void clockTick(long *, struct LatchA *, struct LatchB *, struct LatchC *, struct LatchD *);

int main1(int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long pgm_c=0;//program counter
	long sim_cycle=0;//simulation cycle counter
	//define your own counter for the usage of each pipeline stage here
    int ifUtil, idUtil, exUtil, memUtil, wbUtil;
	
	int test_counter=0;
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for (i = 1; i < argc; i++){
		printf("%s ", argv[i]);
	}
	printf("\n");
	if (argc==7){
		if (strcmp("-s",argv[1])==0){
			sim_mode=SINGLE;
		} else if (strcmp("-b",argv[1])==0){
			sim_mode=BATCH;
		} else {
			printf("Wrong sim mode chosen\n");
			exit(0);
		}
		
		m=atoi(argv[2]);
		n=atoi(argv[3]);
		c=atoi(argv[4]);
		input=fopen(argv[5],"r");
		output=fopen(argv[6],"w");
	} else{
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
	if (sim_mode == 1){
		for (i = 0; i < REG_NUM; i++){
			registers[i].value = 0;
            registers[i].flag = true;
		}
	}
	
	//start your code from here

	//File read
	char *line = malloc(sizeof(char) * 136);
	while (fgets(line, 100, input) != NULL) {
		struct Instruction *inst = stringToInstruction(line);
        if (verifyInstruction(inst)) {
            instructionMem[i++] = *inst;
        } else {
            printf("Instruction is not valid: %s", line);
            exit(0);
        }
	}
    free(line);
    
    struct LatchA *stateA = malloc(sizeof(struct LatchA));
    struct LatchB *stateB = malloc(sizeof(struct LatchB));
    struct LatchC *stateC = malloc(sizeof(struct LatchC));
    struct LatchD *stateD = malloc(sizeof(struct LatchD));

    int running;
    while (running) {
        clockTick(&pgm_c, stateA, stateB, stateC, stateD);
    }
    
    //output code 2: the following code will output the register 
    //value to screen at every cycle and wait for the ENTER key
    //to be pressed; this will make it proceed to the next cycle 
	printf("cycle: %d ",sim_cycle);
	if(sim_mode==1){
		for (i=1;i<REG_NUM;i++){
			printf("%d  ", registers[i].value);
		}
	}
	printf("%d\n",pgm_c);
	pgm_c+=4;
	sim_cycle+=1;
	test_counter++;
	printf("press ENTER to continue\n");
	while(getchar() != '\n');
    
    //add the following code to the end of the simulation, 
    //to output statistics in batch mode
	if(sim_mode==0){
		fprintf(output,"program name: %s\n",argv[5]);
		fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n",
                             ifUtil, idUtil, exUtil, memUtil, wbUtil);
                     // add the (double) stage_counter/sim_cycle for each 
                     // stage following sequence IF ID EX MEM WB
		
		fprintf(output,"register values ");
		for (i=1;i<REG_NUM;i++){
			fprintf(output, "%d  ", registers[i].value);
		}
		fprintf(output, "%d\n", pgm_c);
	}
    
	//close input and output files at the end of the simulation
	fclose(input);
	fclose(output);
	return 0;
}
// end main

//takes in the tokens of an instruction as an array, returns a struct Instruction
struct Instruction *stringToInstruction(char* line){
    char *tokens[6];
    tokens[0] = strtok(line, " ,;)");
    int i = 0;
    while (tokens[i] != NULL && i <= 6) { //puts tokens into array, to be put into an struct Instruction
        tokens[++i] = strtok(NULL, " ,;)");
    }

	struct Instruction *a = malloc(sizeof(struct Instruction));

	if (strcmp(tokens[0], "add") == 0) {
        a->opcode = add;
        a->rd = registerStringtoInt(tokens[1]);
        a->rs = registerStringtoInt(tokens[2]); 
        a->rt = registerStringtoInt(tokens[3]);
    } else if (strcmp(tokens[0], "sub") == 0) {
        a->opcode = sub;
        a->rd = registerStringtoInt(tokens[1]); 
        a->rs = registerStringtoInt(tokens[2]); 
        a->rt = registerStringtoInt(tokens[3]);
    } else if (strcmp(tokens[0], "addi") == 0) {
        a->opcode = addi;
        a->rt = registerStringtoInt(tokens[1]); 
        a->rs = registerStringtoInt(tokens[2]); 
        a->immediate = atoi(tokens[3]);
    } else if (strcmp(tokens[0], "mul") == 0) {
        a->opcode = mul;
        a->rd = registerStringtoInt(tokens[1]); 
        a->rs = registerStringtoInt(tokens[2]); 
        a->rt = registerStringtoInt(tokens[3]);
    } else if (strcmp(tokens[0], "lw") == 0) {
        a->opcode = lw;
        a->rt = registerStringtoInt(tokens[1]); 
        a->immediate = atoi(tokens[2]);
        a->rs = registerStringtoInt(tokens[3]);
    } else if (strcmp(tokens[0], "sw") == 0) {
        a->opcode = sw;
        a->rt = registerStringtoInt(tokens[1]); 
        a->immediate = atoi(tokens[2]);
        a->rs = registerStringtoInt(tokens[3]);
    } else if (strcmp(tokens[0], "beq") == 0) {
        a->opcode = beq;
        a->rs = registerStringtoInt(tokens[1]); 
        a->rt = registerStringtoInt(tokens[2]); 
        a->immediate = atoi(tokens[3]);
    }
    
    return a;
}

//takes in register string returns register number, to be used as index for registers[] array
int registerStringtoInt(char *s) {
    int i;
    char regstr[strlen(s) + 1];
    for (i = 0; s[i]; i++) {
        regstr[i] = tolower(s[i]);
    }
    regstr[i] = s[i];

	char* regNames[] = {"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

	int reg = -1;
	if(sscanf(regstr, "%d", &reg) != 1){ //if its not a string version of an integer (if it is, the int was assigned to reg)
		for (i = 0; i < 32; i++){
			if (strcmp(regNames[i], regstr + 1) == 0) {
                reg = i;
            } //they are the same
		}
	}
    
    // catches the extra case for "$0"
    if (reg == -1 && strcmp("$0", regstr) == 0) {
        reg = 0;
    }
	
	return reg;
}

void clockTick(long *pc, struct LatchA *stateA, struct LatchB *stateB, struct LatchC *stateC, struct LatchD *stateD) {
    writeBack(stateD);
    free(stateD);
    stateD = memory(stateC);
    free(stateC);
    stateC = execute(stateB);
    free(stateB);
    stateB = instructionDecode(stateA);
    free(stateA);
    stateA = instructionFetch(*pc);
    (*pc)++;
}
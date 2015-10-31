// Authors: Aaron Lucia, Sarah Mangels, Matteo Puzella

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

struct Instruction *stringToInstruction(char*);
int registerStringtoInt(char*);
void clockTick(long*, struct LatchA*, struct LatchB*, struct LatchC*, struct LatchD*, int, int, int);

int main(int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long pgm_c=0;//program counter
	long sim_cycle=0;//simulation cycle counter
	//define your own counter for the usage of each pipeline stage here
    double ifUtil, idUtil, exUtil, memUtil, wbUtil;
	
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
    i = 0;
	while (fgets(line, 136, input)) {
        char *pos;
        if ((pos = strchr(line, '\n')) != NULL) {
            *pos = '\0';
        }
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
    stateA->instruction.opcode = add;
    stateA->instruction.rs = 0; 
    stateA->instruction.rt = 0;
    stateA->instruction.rd = 0;
    stateA->instruction.immediate = 0;
    stateA->cycles = 0;
    
    struct LatchB *stateB = malloc(sizeof(struct LatchB));
    stateB->opcode = add;
    stateB->reg1 = 0;
    stateB->reg2 = 0;
    stateB->regResult = 0;
    stateB->immediate = 0;
    stateB->cycles = 0;
    
    struct LatchC *stateC = malloc(sizeof(struct LatchC));
    stateC->opcode = add;
    stateC->reg2 = 0;
    stateC->regResult = 0;
    stateC->result = 0;
    stateC->cycles = 0;
    
    struct LatchD *stateD = malloc(sizeof(struct LatchD));
    stateD->opcode = add;
    stateD->regResult = 0;
    stateD->result = 0;

    if (sim_mode == SINGLE) {
        int running;
        while (running) {
            clockTick(&pgm_c, stateA, stateB, stateC, stateD, c, m, n);
            sim_cycle+=1;

            //output code 2: the following code will output the register 
            //value to screen at every cycle and wait for the ENTER key
            //to be pressed; this will make it proceed to the next cycle 
            printf("cycle: %ld\n",sim_cycle);
            printf("program counter: %ld\n\n",pgm_c);
            for (i=1;i<REG_NUM;i++){
                printf("%2d: %d\t%s\n", i, registers[i].value, registers[i].flag ? "true" : "false");
            }
            printf("press ENTER to continue\n");
            while(getchar() != '\n');
        }
    } else if (sim_mode == BATCH) {
        //add the following code to the end of the simulation, 
        //to output statistics in batch mode
		fprintf(output,"program name: %s\n",argv[5]);
		fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n",
                             ifUtil, idUtil, exUtil, memUtil, wbUtil);
                     // add the (double) stage_counter/sim_cycle for each 
                     // stage following sequence IF ID EX MEM WB
		
		fprintf(output,"register values ");
		for (i=1;i<REG_NUM;i++){
			fprintf(output, "%d  ", registers[i].value);
		}
		fprintf(output, "%ld\n", pgm_c);
	}
    
	//close input and output files at the end of the simulation
	fclose(input);
	fclose(output);
	return 0;
}
// end main

//takes in the tokens of an instruction as an array, returns a struct Instruction
struct Instruction *stringToInstruction(char* line) {
    char *tokens[6];
    char* lineCopy;
    strcpy(lineCopy, line);
    tokens[0] = strtok(lineCopy, " ,;)");
    int i = 0;
    while (tokens[i] != NULL && i < 5) { //puts tokens into array, to be put into an struct Instruction
        tokens[++i] = strtok(NULL, " ,;)");
    }

	struct Instruction *a = malloc(sizeof(struct Instruction));

	if (strcmp(tokens[0], "add") == 0) {
        a->opcode = add;
        a->rs = registerStringtoInt(tokens[2]); 
        a->rt = registerStringtoInt(tokens[3]);
        a->rd = registerStringtoInt(tokens[1]);
        a->immediate = 0;
    } else if (strcmp(tokens[0], "sub") == 0) {
        a->opcode = sub;
        a->rs = registerStringtoInt(tokens[2]); 
        a->rt = registerStringtoInt(tokens[3]);
        a->rd = registerStringtoInt(tokens[1]);
        a->immediate = 0;
    } else if (strcmp(tokens[0], "addi") == 0) {
        a->opcode = addi;
        a->rs = registerStringtoInt(tokens[2]);
        a->rt = registerStringtoInt(tokens[1]);
        a->rd = 0;
        a->immediate = atoi(tokens[3]);
    } else if (strcmp(tokens[0], "mul") == 0) {
        a->opcode = mul;
        a->rs = registerStringtoInt(tokens[2]); 
        a->rt = registerStringtoInt(tokens[3]);
        a->rd = registerStringtoInt(tokens[1]);
        a->immediate = 0;
    } else if (strcmp(tokens[0], "lw") == 0) {
        a->opcode = lw;
        a->rs = registerStringtoInt(tokens[3]);
        a->rt = registerStringtoInt(tokens[1]);
        a->rd = 0;
        a->immediate = atoi(tokens[2]);
    } else if (strcmp(tokens[0], "sw") == 0) {
        a->opcode = sw;
        a->rs = registerStringtoInt(tokens[3]);
        a->rt = registerStringtoInt(tokens[1]);
        a->rd = 0;
        a->immediate = atoi(tokens[2]);
    } else if (strcmp(tokens[0], "beq") == 0) {
        a->opcode = beq;
        a->rs = registerStringtoInt(tokens[1]);
        a->rt = registerStringtoInt(tokens[2]);
        a->rd = 0;
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
		for (i = 0; i < 32; i++) {
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

void clockTick(long *pc, struct LatchA *stateA, struct LatchB *stateB, struct LatchC *stateC, struct LatchD *stateD, int c, int m, int n) {
    if (writeBack(stateD)) {
        if (memory(stateC, stateD, c)) {
            if (execute(stateB, stateC, pc, m, n)) {
                if (instructionDecode(stateA, stateB)) {
                    if (instructionFetch(*pc, stateA)) {
                        printf("pc++\n");
                        (*pc)++;
                    }
                }
            }
        }
    }
}
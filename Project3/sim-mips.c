// Authors: Aaron Lucia, Sarah Mangels, Matteo Puzella
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#include "structs.c"
#include "verify.c"

#define SINGLE 1
#define BATCH 0
#define REG_NUM 32

struct Instruction *stringToInstruction(char*);
int registerStringtoInt(char*);
bool clockTick(long*, struct LatchA*, struct LatchB*, struct LatchC*, struct LatchD*, int, int, int);

bool instructionFetch(int, struct LatchA*);
bool instructionDecode(struct LatchA*, struct LatchB*);
bool execute(struct LatchB*, struct LatchC*, long*, int, int);
bool memory(struct LatchC*, struct LatchD*, int);
bool writeBack(struct LatchD*);

//Array of registers
struct Register registers[REG_NUM];
//Instruction memory
struct Instruction instructionMem[1024];
//Data memory
int dataMem[1024];

//define your own counter for the usage of each pipeline stage here
int ifCounter = 0;
int idCounter = 0;
int exCounter = 0;
int memCounter = 0;
int wbCounter = 0;

int main(int argc, char *argv[]){
	int sim_mode=0;//mode flag, 1 for single-cycle, 0 for batch
	int c,m,n;
	int i;//for loop counter
	long pgm_c = 0;//program counter
	long sim_cycle = 0;//simulation cycle counter
	
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
		printf("Usage: ./sim-mips -s m n c input_name output_name (single-cycle mode)\n or \n ./sim-mips -b m n c input_name  output_name(batch mode)\n");
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
    for (i = 0; i < REG_NUM; i++){
        registers[i].value = 0;
        registers[i].flag = true;
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
            printf("Instruction is not valid: %s\n", line);
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
    
    while (true) {
        if (writeBack(stateD)) {
            if (memory(stateC, stateD, c)) {
                if (execute(stateB, stateC, &pgm_c, m, n)) {
                    if (instructionDecode(stateA, stateB)) {
                        if (instructionFetch(pgm_c, stateA)) {
                            pgm_c += 4;
                        }
                    }
                }
            }
        } else {
            break;
        }
        
        if (sim_mode == SINGLE) {
            //output code 2: the following code will output the register 
            //value to screen at every cycle and wait for the ENTER key
            //to be pressed; this will make it proceed to the next cycle 
            printf("cycle: %d ",sim_cycle);
            for (i = 1; i < REG_NUM; i++){
                printf("%2d: %d\t%s\n", i, registers[i].value, registers[i].flag ? "true" : "false");
            }
            printf("%d\n", pgm_c);
            printf("press ENTER to continue\n");
            while(getchar() != '\n');
        }
            
        sim_cycle += 1;
    }

    if (sim_mode == BATCH) {
        //add the following code to the end of the simulation, 
        //to output statistics in batch mode
		fprintf(output,"program name: %s\n",argv[5]);
        
        // add the (double) stage_counter/sim_cycle for each 
        // stage following sequence IF ID EX MEM WB
        double ifUtil = (double) ifCounter / sim_cycle;
        double idUtil = (double) idCounter / sim_cycle;
        double exUtil = (double) exCounter / sim_cycle;
        double memUtil = (double) memCounter / sim_cycle;
        double wbUtil = (double) wbCounter / sim_cycle;
		fprintf(output,"stage utilization: %f  %f  %f  %f  %f \n",
            ifUtil, idUtil, exUtil, memUtil, wbUtil);
		
		fprintf(output,"register values ");
		for (i = 1; i < REG_NUM; i++){
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
    char *lineCopy = malloc(sizeof(char) * 136);
    strcpy(lineCopy, line);
    tokens[0] = strtok(lineCopy, " ,;()");
    int i = 0;
    while (tokens[i] != NULL && i < 5) { //puts tokens into array, to be put into an struct Instruction
        tokens[++i] = strtok(NULL, " ,;()");
    }

	struct Instruction *a = malloc(sizeof(struct Instruction));

    if (strcmp(tokens[0], "haltSimulation") == 0) {
        a->opcode = haltSimulation;
        a->rs = 0; 
        a->rt = 0;
        a->rd = 0;
        a->immediate = 0;
    } else if (strcmp(tokens[0], "add") == 0) {
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

bool clockTick(long *pc, struct LatchA *stateA, struct LatchB *stateB, struct LatchC *stateC, struct LatchD *stateD, int c, int m, int n) {
    if (writeBack(stateD)) {
        if (memory(stateC, stateD, c)) {
            if (execute(stateB, stateC, pc, m, n)) {
                if (instructionDecode(stateA, stateB)) {
                    if (instructionFetch(*pc, stateA)) {
                        (*pc)++;
                    }
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

/**
 * Stages
 */

// IF
bool instructionFetch(int pc, struct LatchA *result) {
    struct Instruction inst = instructionMem[pc / 4];
    if (inst.opcode == haltSimulation) {
        result->instruction.opcode = haltSimulation;
        result->instruction.rt = 0;
        result->instruction.rs = 0;
        result->instruction.rd = 0;
        result->instruction.immediate = 0;
        return false;
    }
    ifCounter++;
    
    if (result->cycles == 0) {
        if (inst.opcode == beq) {
            result->cycles = 3;
        } else {
            result->cycles = 1;
        }
    }
    result->cycles--;
    if (result->cycles == 0) {
        result->instruction = inst;
        return true;
    } else if(inst.opcode == beq) {
        result->instruction = inst;
        return false;
    } else {
        result->instruction.opcode = add;
        result->instruction.rt = 0;
        result->instruction.rs = 0;
        result->instruction.rd = 0;
        result->instruction.immediate = 0;
        return false;
    }
}

// ID
bool instructionDecode(struct LatchA *state, struct LatchB *result) {
    if (state->instruction.opcode == haltSimulation) {
        result->opcode = haltSimulation;
        result->reg1 = 0;
        result->reg2 = 0;
        result->regResult = 0;
        result->immediate = 0;
        return false;
    }
    idCounter++;
    
    bool readRT;
    switch (state->instruction.opcode) {
        case add:
        case sub:
        case mul:
        case beq:
        case sw:
            readRT = true;
            break; 
        case addi:
        case lw:
            readRT = false;
            break;
    }
    // Check that registers are unflagged
    // flag == false, register not safe
    if (registers[state->instruction.rs].flag && (!readRT || registers[state->instruction.rt].flag)) {
        //flag == true, registers safe
        result->opcode = state->instruction.opcode; 
        result->reg1 = registers[state->instruction.rs].value; //pass reg value
        if (readRT) {
            result->reg2 = registers[state->instruction.rt].value; //pass reg value
        } else {
            result->reg2 = 0;
        }
        switch (state->instruction.opcode) {
            case add:
            case sub:
            case mul:
                result->regResult = state->instruction.rd;
                break; 
            case addi:
            case lw:
                result->regResult = state->instruction.rt;
                break;
            default:
                result->regResult = 0;
                break;
        }
        
        //set write register flags to false (not safe)
        if (result->regResult != 0) {
            registers[result->regResult].flag = false;
        }
        result->immediate = state->instruction.immediate;
        
        return true;
    } else {
        // send NOP add $0, $s0, $s0
        result->opcode = add;
        result->reg1 = 0;
        result->reg2 = 0;
        result->regResult = 0;
        result->immediate = 0;
        
        return false;
    }
}

// EXE
bool execute(struct LatchB *state, struct LatchC *result, long *pc, int multiplyCycles, int otherCycles) {
    if (state->opcode == haltSimulation) {
        result->opcode = haltSimulation;
        result->reg2 = 0;
        result->regResult = 0;
        result->result = 0;
        return false;
    }
    exCounter++;
    
    if (state->cycles == 0) {
        if (state->opcode == mul) {
            state->cycles = multiplyCycles;
        } else {
            state->cycles = otherCycles;
        }
    }
    
    int aluResult = 0;
    switch (state->opcode) {
    case add:
        aluResult = state->reg1 + state->reg2;
        break;
    case sub:
    case beq:
        aluResult = state->reg1 - state->reg2;
        break;
    case mul:
        aluResult = state->reg1 * state->reg2;
        break;
    case addi:
    case sw:
    case lw:
        aluResult = state->reg1 + state->immediate;
        break;
    }
    
    state->cycles--;
    if (state->cycles == 0) {
        if (state->opcode == beq && aluResult == 0) {
            (*pc) += state->immediate;
        }
        result->opcode = state->opcode;
        result->reg2 = state->reg2;
        result->regResult = state->regResult;
        result->result = aluResult;
        return true;
    } else {
        return false;
    }
}

// MEM
bool memory(struct LatchC *state, struct LatchD *result, int accessCycles) {
    if (state->opcode == haltSimulation) {
        result->opcode = haltSimulation;
        result->regResult = 0;
        result->result = 0;
        return false;
    }
    memCounter++;
    
    if (state->cycles == 0) {
        switch (state->opcode) {
        case sw:
        case lw:
            state->cycles = accessCycles;
            break;
        default:
            state->cycles = 1;
            break;
        }
    }
    int memResult = 0;
    switch (state->opcode) {
    case sw:
        dataMem[state->result] = state->reg2;
        break;
    case lw:
        memResult = dataMem[state->result];
        break;
    default:
        memResult = state->result;
        break;
    }
    
    state->cycles--;
    if (state->cycles == 0) {
        result->opcode = state->opcode;
        result->regResult = state->regResult;
        result->result = memResult;
        return true;
    } else {
        return false;
    }
}

// WB
bool writeBack(struct LatchD *state) {
    if (state->opcode == haltSimulation){
        return false;
    }
    wbCounter++;
    
    switch (state->opcode) {
    case add:
    case sub:
    case addi:
    case lw:
    case mul:
        if (state->regResult != 0) {
            registers[state->regResult].value = state->result;
            registers[state->regResult].flag = true;
        }
        break;
    default:
        break;
    }
    
    return true;
}

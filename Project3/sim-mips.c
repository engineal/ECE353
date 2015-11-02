// Authors: Aaron Lucia, Sarah Mangels, Matteo Puzella
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>

#define SINGLE 1
#define BATCH 0
#define REG_NUM 32
#define MEM_SIZE 1024

typedef enum {
    add, sub, addi, mul, lw, sw, beq, haltSimulation
} Opcode;

struct Instruction {
    Opcode opcode;
    int rs;
    int rt;
    int rd;
    int immediate;
};

struct Register {
	int value; 
	bool flag; // if flag == true, the register is safe
};

struct LatchA {
    struct Instruction instruction;
    
    int cycles;
};

struct LatchB {
    Opcode opcode;
    int reg1; //reg value
    int reg2; //reg value
    int regResult;
    int immediate;
    
    int cycles;
};

struct LatchC {
    Opcode opcode;
    int reg2;
    int regResult;
    int result;
    
    int cycles;
};

struct LatchD {
    Opcode opcode;
    int regResult;
    int result;
};

struct Instruction *stringToInstruction(char*);
int registerStringtoInt(char*);
int verifyInstruction(struct Instruction*);

bool instructionFetch(struct LatchA*, int);
bool instructionDecode(struct LatchA*, struct LatchB*);
bool execute(struct LatchB*, struct LatchC*, int, int);
bool memory(struct LatchC*, struct LatchD*, int);
bool writeBack(struct LatchD*);

long pgm_c = 0;//program counter
//Array of registers
struct Register registers[REG_NUM];
//Instruction memory
struct Instruction instructionMem[MEM_SIZE];
//Data memory
int dataMem[MEM_SIZE];

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
	long sim_cycle = 0;//simulation cycle counter
	
	FILE *input=NULL;
	FILE *output=NULL;
	printf("The arguments are:");
	
	for (i = 1; i < argc; i++){
		printf("%s ", argv[i]);
	}
	printf("\n");
	if (argc==7){
		if (strcmp("-s", argv[1])==0){
			sim_mode = SINGLE;
		} else if (strcmp("-b", argv[1])==0){
			sim_mode = BATCH;
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
    bool error = false;
	while (fgets(line, 136, input)) {
        char *pos;
        if ((pos = strchr(line, '\n')) != NULL) {
            *pos = '\0';
        }
		struct Instruction *inst = stringToInstruction(line);
        switch (verifyInstruction(inst)) {
        case 0:
            instructionMem[i] = *inst;
            break;
        case 1:
            fprintf(output, "Illegal opcode on line %d: %s\n", i + 1, line);
            error = true;
            break;
        case 2:
            fprintf(output, "Illegal register on line %d: %s\n", i + 1, line);
            error = true;
            break;
        case 3:
            fprintf(output, "Immediate out of bounds on line %d: %s\n", i + 1, line);
            error = true;
            break;
        }
        i++;
	}
    free(line);
    
    if (error) {
        exit(0);
    }
    
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
                if (execute(stateB, stateC, m, n)) {
                    if (instructionDecode(stateA, stateB)) {
                        if (instructionFetch(stateA, c)) {
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
            printf("cycle: %ld \n",sim_cycle);
            for (i = 1; i < REG_NUM; i++){
                printf("%2d: %d\t%s\n", i, registers[i].value, registers[i].flag ? "true" : "false");
            }
            printf("%ld\n", pgm_c);
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
		fprintf(output, "\n%ld\n", pgm_c);
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
    } else {
        a->opcode = -1;
        a->rs = -1;
        a->rt = -1;
        a->rd = -1;
        a->immediate = -1;
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
	if(sscanf(regstr, "%d", &reg) != 1){            // handles reg # without $
        if (sscanf(regstr + 1, "%d", &reg) == 0) {  // handles reg # with $
            for (i = 0; i < 32; i++) {              // handles reg text with $
                if (strcmp(regNames[i], regstr + 1) == 0) {
                    reg = i;
                }
            }
        }
	}
	
	return reg;
}

int verifyInstruction(struct Instruction *inst) {
    if (inst->opcode > 7) {
        return 1;
    }
    if (inst->rs < 0 || inst->rs > 31 ||
        inst->rt < 0 || inst->rt > 31 ||
        inst->rd < 0 || inst->rd > 31) {
        return 2;
    }
    if ((((((inst->immediate) & 0xffff8000) >> 15) + 1) & 0x1fffe)) {
        return 3;
    }
    return 0;
}

/**
 * Stages
 */

// IF
bool instructionFetch(struct LatchA *result, int accessCycles) {
    struct Instruction inst = instructionMem[pgm_c / 4];
    if (inst.opcode == haltSimulation) {
        result->instruction = inst;
        return false;
    }
    ifCounter++;
    
    if (result->cycles == 0) {
        result->cycles = accessCycles;
    }
    result->cycles--;
    if (result->cycles == 0) {
        result->instruction = inst;
        return true;
    } else {
        result->instruction.opcode = 0;
        result->instruction.rs = 0;
        result->instruction.rt = 0;
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
        case haltSimulation:
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
        
        if (state->instruction.opcode == beq) {
            state->instruction.opcode = add;
            state->instruction.rs = 0;
            state->instruction.rt = 0;
            state->instruction.rd = 0;
            state->instruction.immediate = 0;
            return false;
        } else {
            return true;
        }
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
bool execute(struct LatchB *state, struct LatchC *result, int multiplyCycles, int otherCycles) {
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
    default:
        break;
    }
    
    state->cycles--;
    if (state->cycles == 0) {
        if (state->opcode == beq && aluResult == 0) {
            pgm_c += (state->immediate << 2);
            assert(pgm_c > 0 && pgm_c < MEM_SIZE);
        }
        result->opcode = state->opcode;
        result->reg2 = state->reg2;
        result->regResult = state->regResult;
        result->result = aluResult;
        return true;
    } else {
        result->opcode = add;
        result->reg2 = 0;
        result->regResult = 0;
        result->result = 0;
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
        assert(state->result % 4 == 0);
        dataMem[state->result / 4] = state->reg2;
        break;
    case lw:
        assert(state->result % 4 == 0);
        memResult = dataMem[state->result / 4];
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
        result->opcode = add;
        result->regResult = 0;
        result->result = 0;
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

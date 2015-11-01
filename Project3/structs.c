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

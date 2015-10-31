typedef enum {
    add, sub, addi, mul, lw, sw, beq
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
};

struct LatchB {
    Opcode opcode;
    int rd; //reg#
    int reg1; //reg value
    int reg2; //reg value
    int immediate;
    
    int cycles;
};

struct LatchC {
    Opcode opcode;
    int rd;
    int reg2;
    int result;
    
    int cycles;
};

struct LatchD {
    Opcode opcode;
    int rd;
    int result;
};

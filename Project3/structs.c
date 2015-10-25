typedef enum {
    add, sub, addi, mul, lw, sw, beq
} Opcode ;

struct Instruction {
    Opcode opcode;
    int rs;
    int rt;
    int rd;
};

struct Register {
	int value; 
	boolean flag; // if flag == true, the register is safe
}; 

int PC;

struct LatchA {
    struct Instruction instruction;
    boolean ready;
};

struct LatchB {
    Opcode opcode;
    int rd; //reg#
    int reg1; //reg value
    int reg2; //reg value
    boolean ready;
};

struct LatchC {
    Opcode opcode;
    int rd;
    int result;
    boolean ready;
};

struct LatchD {
    Opcode opcode;
    int rd;
    int result;
    boolean ready;
};

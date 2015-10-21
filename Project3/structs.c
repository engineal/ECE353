enum Opcode {
    add, sub, mul, sw, lw;
};

struct Instruction {
    Opcode opcode;
    int rs;
    int rt;
    int rd;
};

int PC;

struct LatchA {
    struct Instruction instruction;
    boolean ready;
};

struct LatchB {
    Opcode opcode;
    int rd;
    int reg1;
    int reg2;
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

//IF Stage
int pc;
struct Instruction instructions[];

void IF{
LatchA.instruction = instructions[pc];
}

//WB Stage
void WB {
registers[LatchD.rd] =LatchD.result ;
}

//struct Register Registers[32]; 


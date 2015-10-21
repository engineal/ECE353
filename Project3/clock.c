int PC;
struct LatchA *stateA = malloc(sizeof(struct LatchA));
struct LatchB *stateB = malloc(sizeof(struct LatchB));
struct LatchC *stateC = malloc(sizeof(struct LatchC));
struct LatchD *stateD = malloc(sizeof(struct LatchD));

int running;
while (running) {
    writeBack(stateD);
    stateD = memory(stateC);
    stateC = execute(stateB);
    stateB = instructionDecode(stateA);
    stateA = instructionFetch(PC);
}
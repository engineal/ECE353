int PC;
struct LatchA *stateA = malloc(sizeof(struct LatchA));
struct LatchB *stateB = malloc(sizeof(struct LatchB));
struct LatchC *stateC = malloc(sizeof(struct LatchC));
struct LatchD *stateD = malloc(sizeof(struct LatchD));

int running;
while (running) {
    writeBack(stateD);
    free(stateD);
    stateD = memory(stateC);
    free(stateC);
    stateC = execute(stateB);
    free(stateB);
    stateB = instructionDecode(stateA);
    free(stateA);
    stateA = instructionFetch(PC);
    PC++;
}
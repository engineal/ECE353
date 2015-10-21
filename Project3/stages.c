// IF

// ID
void ID(){
	// Check that registers are unflagged
	if ( LatchA.instruction.rs.flag == 1 ||
	LatchA.instruction.rt.flag == 1 ) {
		
		// send NOP add $0, $s0, $s0
		// Set LatchA ready value to…..? 
	}
	else{
		LatchB.opcode = LatchA.instruction.opcode; 
		//pass registers by value or by reg#???? 
		LatchB.ready = 1;  
	}

	
}
// EXE

// MEM

// WB


//Array of registers
struct Register Registers[32]; 


//takes in register string returns register number, to be used as index for Registers[] array
int RegisterStringtoInt(char *s){

char* regs = [ “$s0

}
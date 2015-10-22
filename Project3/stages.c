// IF

// ID
void ID(){
	// Check that registers are unflagged
	//flag == false, register not safe
	if ( LatchA.instruction.rs.flag == false ||
	LatchA.instruction.rt.flag == false ) {
		
		// send NOP add $0, $s0, $s0
		LatchB.opcode = add; 
		LatchB.rd = 0; 
		LatchB.reg1 = 0; 
		LatchB.reg2 = 0;  
		
		// Set LatchA ready value to…..? 
		LatchA.ready = true; 

	}
	//flag == true, registers safe
	else{
		LatchB.opcode = LatchA.instruction.opcode; 
		LatchB.rd = LatchA.instruction.rd; //pass reg #
		LatchB.reg1 = Registers[LatchA.instruction.rs]; //pass reg value
		LatchB.reg2 =  Registers[LatchA.instruction.rt]; //pass reg value

		LatchB.ready = true;  
	}

	
}
// EXE

// MEM

// WB


//Array of registers
struct Register Registers[32]; 


//takes in register string returns register number, to be used as index for Registers[] array
int RegisterStringtoInt(char *s){
	s =tolower(s); 
	char* regNames = ["zero", "at", "v0", "v1", "a0", "a1", "a2", "a3", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"]; 

	int reg; 
	if(sscanf(s, "%d", &reg)!=1){ //if its not a string version of an integer (if it is, the int was assigned to reg)
		for(int i = 0; i<32; i++){
			if (strcmp(regNames[i], s) == 0) { reg = i; } //they are the same
		}
	}

	//make sure regs is within 
	assert(reg<32); 
	assert(reg<=0); 

	return reg; 

	/* to do : what if the register in invalid? ie out of 0-31 or a word etc*/	
}





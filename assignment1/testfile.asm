R:	add $s0, $s1, $s2	#r instruction
	add $s0, $s1, $s2
	sub s0, $s1, $s2  
	subu $s0, $s1, $s2
	and $s0, $s1, $s2
	or $s0, $s1, $s2    
	nor $s0, $s1, $s2
        xor $t0, $t1, $t2
	div $t1, $t2
	divu $t4, $t2
	sll $s0, $s1, 10
	srl $s0, $s1, 10
	slt $s0, $s1, $s3
	sltu $s0, $s1, $s3
	jr $ra
	mfhi $t3
	mflo $s2
	mult $t2, $t3
	multu $t1, $t0
	


I: addi $s0, $s1, -100	 
	addiu $s0, $s1, 100
	andi $s0, $s1, 100


	ori $s0, $s1, 100
	slti $s0, $s1, 100
	sltiu $s0, $s1, 100
	beq $s0, $s1, I
	bne $s0, $s1, R
	bgez $s0, I
	bltz $s1, I 
	lw $s0, 100($s1)	

	sw $s0, 100($s1)	#123
	lui $s0, 100	#2




J:
	j I			
	jal R
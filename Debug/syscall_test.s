#save all the registers
	mfc0 $k0, $cr
	mfc0 $k1, $sr
	and	$k0, $k0, $k1
	ori	$k1, $k0, 8192	#0X2000
	bne	$k1, $zero, interrupt
	ori	$k1, $k0, 32768
	beq	$k1, $zero, syscall

sys_done:	#moved ahead for la
	li	$k0, 0
	mtc0 $k0, $cr	#clear cause

	mfc0 $k0, $sr
	ori $k0, $k0, 2
	mtc0 $k0, $sr	#set Status EXL enable 

	mfc0 $ra, $14	#EPC
	jr	$ra
syscall:
	la	$ra, sys_done
	li	$t0, 11
	beq	$v0, $t0, sys_print_ch #$v0 = 1
	li 	$t0, 4	# $t0 = 4
	beq 	$v0, $t0, sys_print_str # if $v0 = 4
	li	$t0, 1
	beq	$v0, $t0, sys_print_int #$v0 = 1
	j	sys_done

sys_print_int:	#print $a0
	move	$t4, $zero	#t4 = 0
	addi	$sp, $sp, -1 	#sp-=4
	sb		$zero, 0($sp)	# '\0'
	addi	$t4, $t4, 1
	addi	$t1, $zero, 10	#const t1 = 10
	int_loop:
		divu 	$a0, $t1
		mflo 	$t2	
		mfhi	$t0			#t0 = a0 % 10
		addi	$t0, $t0, 48	#'0'
		addi	$sp, $sp, -1 	#sp -= 1
		sb		$t0, 0($sp)		#push	
		addi	$t4, $t4, 1		#count
		add		$a0, $t2, $zero	#a0 = t2
		bne		$a0, $zero, int_loop
	add		$v0, $zero, $sp	#v0 = sp
	addi	$sp, $sp, -1
	sb 	$ra, 0($sp)
	jal	sys_print_str
	lb	$ra, 0($sp)
	add		$sp, $sp, $t4	#pop all
	j	sys_done
sys_print_str:	#print $a0
	lb 	$s0, 0($a0)
	beq $s0, $zero, sys_done
	push $a0
	jal sys_print_ch
	pop	$a0
	addi $a0, $a0, 1
	j	sys_print_str
sys_print_ch:		#a0 the char to output
	lui		$t0, 11
	addiu	$t0, $t0, 32764	#t0: addr for cursor
	addi	$t1, $t0, 4		#t1: addr for disp	
	lw		$t2, 0($t0)		#cursor value

	li		$t3, 10	#0A new line 
	beq		$a0, $t3, sys_new_line
	add		$t3, $t2, $t1	#addr for char
	sw		$a0, 0($t3)	#putchar

	addi	$t2, $t2, 1
	sw		$t2, 0($t0)	#cursor++	
	jr		$ra
sys_new_line:
	li		$t3, 80
	divu	$t2, $t3	#cursor/80
	mfhi	$t2
	addi	$t2, $t2, 1	#row++
	multu	$t2, $t2, $t3	#row*80
	sw		$t2, 0($t0)	#update cursor value
	jr		$ra

interrupt:	#do nothing right now
	jr		$ra

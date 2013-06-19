	addi $sp, $sp, -140 
	sw	$at, 0($sp)
	sw	$v0, 4($sp)
	sw  $v1, 8($sp)
	sw	$a0, 12($sp)
	sw	$a1, 16($sp)
	sw	$a2, 20($sp)
	sw	$a3, 24($sp)
	sw	$t0, 28($sp)
	sw	$t1, 32($sp)
	sw	$t2, 36($sp)
	sw	$t3, 40($sp)
	sw	$t4, 44($sp)
	sw	$t5, 48($sp)
	sw	$t6, 52($sp)
	sw	$t7, 56($sp)
	sw	$s0, 60($sp)
	sw	$s1, 64($sp)
	sw	$s2, 68($sp)
	sw	$s3, 72($sp)
	sw	$s4, 76($sp)
	sw	$s5, 80($sp)
	sw	$s6, 84($sp)
	sw	$s7, 88($sp)
	sw	$t8, 92($sp)
	sw	$t9, 96($sp)
	sw	$k0, 100($sp)
	sw	$k1, 104($sp)
	sw	$gp, 108($sp)
	sw	$fp, 116($sp)
	sw	$ra, 120($sp)
	mfhi	$k0
	mflo	$k1
	sw	$k0, 124($sp)	#save mfhi
	sw	$k1, 128($sp)	#save mflo
	mfc0	$k0, $cr
	mfc0	$k1, $sr
	sw  $k0, 132($sp)	#save status
	sw	$k1, 136($sp)	#save cause
	mfc0	$k0, $14
	sw	$k0, 140($sp)	#save cause
#save all the registers

	mfc0 $k0, $cr
	mfc0 $k1, $sr
	and	$k0, $k0, $k1
	andi $k1, $k0, 8192	#0X2000
	bne	$k1, $zero, interrupt
	andi $k1, $k0, 32768	#8000	syscall
	beq $k1, $zero, sys_done	#nothing match
	jal	syscall	# $ar = sys_done		addr 80304
sys_done:	#moved ahead for la
	lw	$k0, 124($sp)	#save mfhi
	lw	$k1, 128($sp)	#save mflo
	mthi	$k0
	mtlo	$k1
	lw  $k0, 132($sp)	#save status
	lw	$k1, 136($sp)	#save cause
	mtc0	$k0, $cr
	mtc0	$k1, $sr
	lw	$k0, 140($sp)
	mtc0	$k0, $14
	lw	$at, 0($sp)
	lw	$v0, 4($sp)
	lw  $v1, 8($sp)
	lw	$a0, 12($sp)
	lw	$a1, 16($sp)
	lw	$a2, 20($sp)
	lw	$a3, 24($sp)
	lw	$t0, 28($sp)
	lw	$t1, 32($sp)
	lw	$t2, 36($sp)
	lw	$t3, 40($sp)
	lw	$t4, 44($sp)
	lw	$t5, 48($sp)
	lw	$t6, 52($sp)
	lw	$t7, 56($sp)
	lw	$s0, 60($sp)
	lw	$s1, 64($sp)
	lw	$s2, 68($sp)
	lw	$s3, 72($sp)
	lw	$s4, 76($sp)
	lw	$s5, 80($sp)
	lw	$s6, 84($sp)
	lw	$s7, 88($sp)
	lw	$t8, 92($sp)
	lw	$t9, 96($sp)
	lw	$k0, 100($sp)
	lw	$k1, 104($sp)
	lw	$gp, 108($sp)
	lw	$fp, 116($sp)
	lw	$ra, 120($sp)
	addi $sp, $sp, 140 

	li	$k0, 0
	mtc0 $k0, $cr	#clear cause

	mfc0 $k0, $sr
	ori $k0, $k0, 2
	mtc0 $k0, $sr	#set Status EXL enable 

	mfc0 $ra, $14	#EPC
	jr	$ra
syscall:
	li	$t0, 11
	beq	$v0, $t0, sys_print_ch #$v0 = 11
	li 	$t0, 4	# $t0 = 4
	beq 	$v0, $t0, sys_print_str # if $v0 = 4
	li	$t0, 1
	beq	$v0, $t0, sys_print_int #$v0 = 1
	j	sys_done

sys_print_int:	#print $a0 addr 80320
	move	$t4, $zero	#t4 = 0
	pushb	$zero	# '\0'
	inc		$t4
	li		$t1, 10			#const t1 = 10
	int_loop:
		divu 	$a0, $t1	#num/10
		mflo 	$t2			#t2 = num/10	
		mfhi	$t0			#t0 = a0 % 10
		addi	$t0, $t0, 48	#'0'
		pushb	$t0
		inc		$t4			#count
		move	$a0, $t2		#a0=num/10
		bne		$a0, $zero, int_loop
	move $a0, $sp	#a0 = sp
	push $ra
	jal	sys_print_str
	pop  $ra
	add		$sp, $sp, $t4	#pop all
	j	sys_done
sys_print_str:	#print $a0
	move $s1, $a0
	push $ra
sys_pstr_loop:
	lb 	$s0, 0($s1)	#$s0 char
	beq $s0, $zero, sys_pstr_brk
	move	$a0, $s0
	jal sys_print_ch
	addi $s1, $s1, 1
	j	sys_pstr_loop
sys_pstr_brk:
	pop	$ra
	jr	$ra
sys_print_ch:		#a0 the char to output
	lui		$t0, 11
	addiu	$t0, $t0, 32764	#t0: addr for cursor
	addi	$t1, $t0, 4		#t1: addr for disp	
	lw		$t2, 0($t0)		#cursor value

	li		$t3, 10	#0A new line 
	beq		$a0, $t3, sys_new_line
	add		$t3, $t2, $t1	#addr for char
	sb		$a0, 0($t3)	#putchar

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
	lui		$t0, 15
	ori		$t0, $t0, 65284 #receive addr FFF04
	lui		$t1, 13		#buf addr 0XD0000
	addi	$t2, $t1, -4	#buf 
#TODO
		
	jr		$ra

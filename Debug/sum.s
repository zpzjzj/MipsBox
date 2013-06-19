li	 $s1, 36	#const s1 = 36  24h
move $s0, $zero		#i = 0
move $s3, $zero #s3 => sum
FOR:
	blt $s1, $s0, FOR_OUT	#if i < 36 not break
	move $a0, $s3			#a0 = i
	li	$v0, 1				#print_int
	syscall
	li $v0, 11
	li	$a0, 32		#' '
	syscall			#putchar
	add $s3, $s3, $s0	#sum
	inc	$s0
	j FOR
FOR_OUT:
	move $a0, $s3
	li	$v0, 1				#print_int
	syscall

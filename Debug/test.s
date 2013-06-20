li	$a0, 65
li	$v0, 11
syscall

li	$v0, 9000
loop:
dec $v0
bne $v0, $zero, loop
li	$a0, 66
li	$v0, 11
syscall
li	$v0, 12
syscall	#getchar

move $a0, $v0
beq $a0, $zero, done #do not putchar
li	$v0, 11
syscall	#putchar
done:
li	$a0, 67
li	$v0, 11
syscall


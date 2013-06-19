lui 	$t1, 11 # $t1 = disp_mem  0XB 8000
addiu	$t1, $t1, 32768
addi	$t2, $zero, 100
loop:
addi	$t3, $zero, 102 #98 : 'b'
sb		$t3, 0($t1)	# disp_mem[ptr] = ch
addi    $t1, $t1, 1
addi 	$t2, $t2, -1
bne 	$t2, $zero, loop

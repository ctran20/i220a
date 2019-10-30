	.text
	.globl get_parity
#edi contains n	
get_parity:

	#@TODO: add code here to set eax to 1 iff edi has even parity
	testl	$0, %edi	
	jpe		.A
	movl	$0, %eax
	ret	
.A:
	movl	$1, %eax
	ret
	

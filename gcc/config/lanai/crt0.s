	!;; This file must run on all versions of the lanai compiler.

.text

.global start
start:
	mov	0x20000,%sp	!; set stack pointer
	mov     %sp,%fp		!; set frame pointer

	!;; call main()

	mov	1f,%rca
	bt	_main
	st	%rca,[--%sp]
	nop
1:

	bt	.		!; trap in infinite loop
	nop
	nop

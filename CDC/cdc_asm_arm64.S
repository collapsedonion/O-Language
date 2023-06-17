
.global _cdc_invoke
.balign 4
_cdc_invoke:
	str x9, [sp, #-16]!
	str x10, [sp, #-16]!
	str x11, [sp, #-16]!
	str x19, [sp, #-16]!
	str x13, [sp, #-16]!
	str x14, [sp, #-16]!
	str x15, [sp, #-16]!
	str x12, [sp, #-16]!
	str x16, [sp, #-16]!
	str x17, [sp, #-16]!
	str lr, [sp, #-16]!
	str fp, [sp, #-16]!
	sub sp, sp, #16

	mov x9, x0
	mov x10, x1
	mov x11, x2
	mov x16, x3

	_if:
	cmp x10, #0
	b.ls _endif
	ldr x0, [x11]
	sub x10, x10, #1
	
	cmp x10, #0
	b.ls _endif
	add x11, x11, #8
	ldr x1, [x11]
	sub x10, x10, #1

	cmp x10, #0
	b.ls _endif
	add x11, x11, #8
	ldr x2, [x11]
	sub x10, x10, #1

	cmp x10, #0
	b.ls _endif
	add x11, x11, #8
	ldr x3, [x11]
	sub x10, x10, #1

	cmp x10, #0
	b.ls _endif
	add x11, x11, #8
	ldr x4, [x11]
	sub x10, x10, #1

	cmp x10, #0
	b.ls _endif
	add x11, x11, #8
	ldr x5, [x11]
	sub x10, x10, #1

	cmp x10, #0
	b.ls _endif
	add x11, x11, #8
	ldr x6, [x11]
	sub x10, x10, #1

	cmp x10, #0
	b.ls _endif
	add x11, x11, #8
	ldr x7, [x11]
	sub x10, x10, #1

	_endif:

	mov x13, x10
	
	mov x19, sp
	
	add x16, x16, #8

	mov x17, #16
	mul x13, x13, x17
	
	sub sp, sp, x13 

	mov x13, sp
	
	_send_cycle:
	cmp x10, #0
	b.ls _send_cycle_end

	ldrb w14, [x16]
	mov x17, #4	
	cmp x14, #'i'
	csel x12, x17, x12, EQ
	mov x17, #2
	cmp x14, #'s'
	csel x12, x17, x12, EQ
	mov x17, #1
	cmp x14, #'c'
	csel x12, x17,x12, EQ
	mov x17, #8
	cmp x14, #'l'
	csel x12, x17, x12, EQ

	_align_:
	
	udiv x17, x13, x12
	msub x17, x17, x12, x13
	cmp x17, #0
	b.eq _align_end_	
	sub x12, x12, x17
	add x13, x13, x12
	
	_align_end_:

	cmp x14, #'i'
	b.eq _send_i_

	cmp x14, #'s'
	b.eq _send_s_

	cmp x14, #'c'
	b.eq _send_c_

	cmp x14, #'l'
	b.eq _send_l_

	_send_i_:
	add x11, x11, #8
	ldr x14, [x11]
	str w14, [x13]
	add x13, x13, #4
	b _next_

	_send_s_:
	add x11, x11, #8
	ldr x14, [x11]
	strh w14, [x13]
	add x13, x13, #2
	b _next_
	
	_send_c_:
	add x11, x11, #8
	ldr x14, [x11]
	strb w14, [x13]
	add x13, x13, #1
	b _next_

	_send_l_:
	add x11, x11, #8
	ldr x14, [x11]
	str x14, [x13]
	add x13, x13, #8
	b _next_

	_next_:	
	add x16, x16, #1 
	sub x10, x10, #1
	b _send_cycle
	_send_cycle_end:

	blr x9

	mov sp, x19

	add sp, sp, #16
	ldr fp, [sp], #16
	ldr lr, [sp], #16
	ldr x17, [sp], #16
	ldr x16, [sp], #16
	ldr x12, [sp], #16
	ldr x15, [sp], #16	
	ldr x14, [sp], #16
	ldr x13, [sp], #16
	ldr x19, [sp], #16
	ldr x11, [sp], #16
	ldr x10, [sp], #16
	ldr x9, [sp], #16
	ret
	

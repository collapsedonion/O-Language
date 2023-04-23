
.intel_syntax noprefix

.text
#ifdef __APPLE__
.global _cdc_invoke
_cdc_invoke:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	mov r10, rdi
	mov r11, rsi
	mov r12, rdx

	mov r13, r11

	._if_:
	cmp r13, 0
	jle ._end_if_
	mov rdi, [r12]
	sub r13, 1
	
	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov rsi, [r12]
	sub r13, 1	

	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov rdx, [r12]
	sub r13, 1	
	
	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov rcx, [r12]
	sub r13, 1	

	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov r8, [r12]
	sub r13, 1	

	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov r9, [r12]
	sub r13, 1	

	._end_if_:
	mov r14, rsp
	
	._al_cycle_:
	cmp r13, 0
	jle ._al_cycle_end_
	sub rsp, 8
	sub r13, 1	
	jmp ._al_cycle_
	._al_cycle_end_:	

	mov r13, r11
	sub r13, 6
	mov r15, rsp

	._send_cycle_:
	cmp r13, 0
	jle ._send_cycle_end_

	add r12, 8
	mov r11, [r12]
	mov [r15], r11
	add r15, 8	
	
	sub r13, 1
	jmp ._send_cycle_	
	._send_cycle_end_:
	
	call r10

	mov rsp, r14

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret
#else
.global cdc_invoke
cdc_invoke:
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	mov r10, rcx
	mov r11, rdx 
	mov r12, r8

	mov r13, r11

	._if_:
	
	cmp r13, 0
	jle ._end_if_
	mov rcx, [r12]
	sub r13, 1

	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov rdx, [r12]
	sub r13, 1

	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov r8, [r12]
	sub r13, 1

	cmp r13, 0
	jle ._end_if_
	add r12, 8
	mov r9, [r12]
	sub r13, 1

	._end_if_:

	mov r14, rsp
	
	._al_cycle_:
	cmp r13, 0
	jle ._al_cycle_end_
	sub rsp, 8
	sub r13, 1	
	jmp ._al_cycle_
	._al_cycle_end_:	

	mov r13, r11
	sub r13, 4
	mov r15, rsp

	._send_cycle_:
	cmp r13, 0
	jle ._send_cycle_end_

	add r12, 8
	mov r11, [r12]
	mov [r15], r11
	add r15, 8	
	
	sub r13, 1
	jmp ._send_cycle_	
	._send_cycle_end_:

	sub rsp, 32
	
	call r10

	mov rsp, r14
		
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	ret
#endif

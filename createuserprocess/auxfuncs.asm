;EXTRN PROC KiUserCallbackDispatcher
.code
syscallStub PROC
	mov eax, ecx
	mov r10, rdx
	mov rdx, r8
	mov r8, r9
	mov r9, qword ptr[rsp+28h]
	add rsp, 8h
	nop
	syscall
	sub rsp, 8h
	ret
syscallStub ENDP
myTerminate PROC
int 29h
myTerminate ENDP
mymemcmp PROC
push rsi
push rdi
mov rsi, rcx
mov rdi, rdx
mov rcx, r8
cld
cmp rcx, rcx
repe cmpsb
setz al
pop rdi
pop rsi
ret
mymemcmp ENDP
;customCallKiUserCallbackDispatcher PROC
;sub rsp, 20h

;(0x67123891237698, 0xaabebadcbaeedba, 0xffffefeeefefefda);
END
.code
MyProc1 proc
add RCX, RDX
mov RAX, RCX
ret
MyProc1 endp

PUBLIC MultiplyAsm

.code

; MultiplyAsm(double* A, double* B, double* C, int rowsA, int colsA, int colsB)
;   RCX = A
;   RDX = B
;   R8  = C
;   R9  = rowsA
;   [rsp+28h] = colsA  (5. arg, 32-bit)
;   [rsp+30h] = colsB  (6. arg, 32-bit)

MultiplyAsm PROC

    ; pobierz 5. i 6. argument 
    mov     r10d, dword ptr [rsp+28h]   ; colsA
    mov     r11d, dword ptr [rsp+30h]   ; colsB

   
    push    rbx
    push    r12
    push    r13
    push    r14

    ; i = 0
    xor     r12, r12

outer_i:
    cmp     r12, r9              ; i < rowsA ?
    jge     done

    ; j = 0
    xor     r13, r13

outer_j:
    cmp     r13, r11             ; j < colsB ?
    jge     next_i

    ; sum = 0.0
    xorpd   xmm0, xmm0

    ; k = 0
    xor     r14, r14

inner_k:
    cmp     r14, r10             ; k < colsA ?
    jge     store_result

    ; A[i*colsA + k]
    mov     rax, r12
    imul    rax, r10             ; i*colsA
    add     rax, r14             ; + k
    shl     rax, 3               ; *8 (double)
    movsd   xmm1, qword ptr [rcx + rax]

    ; B[k*colsB + j]
    mov     rbx, r14
    imul    rbx, r11             ; k*colsB
    add     rbx, r13             ; + j
    shl     rbx, 3               ; *8
    movsd   xmm2, qword ptr [rdx + rbx]

    mulsd   xmm1, xmm2
    addsd   xmm0, xmm1

    inc     r14
    jmp     inner_k

store_result:
    ; C[i*colsB + j] = sum
    mov     rax, r12
    imul    rax, r11             ; i*colsB
    add     rax, r13             ; + j
    shl     rax, 3               ; *8
    movsd   qword ptr [r8 + rax], xmm0

    inc     r13
    jmp     outer_j

next_i:
    inc     r12
    jmp     outer_i

done:
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    ret
MultiplyAsm ENDP
PUBLIC MultiplyAsmPartial

.code

; MultiplyAsmPartial(double* A, double* B, double* C, int rowsA, int colsA, int colsB, int startRow, int endRow)
; RCX = A
; RDX = B
; R8  = C
; R9  = rowsA
; [rsp+28h] = colsA
; [rsp+30h] = colsB
; [rsp+38h] = startRow
; [rsp+40h] = endRow

MultiplyAsmPartial PROC
    ; pobierz argumenty
    mov r10d, dword ptr [rsp+28h]   ; colsA
    mov r11d, dword ptr [rsp+30h]   ; colsB
    mov r12d, dword ptr [rsp+38h]   ; startRow
    mov r13d, dword ptr [rsp+40h]   ; endRow

    push rbx
    push r14
    push r15

outer_i:
    cmp r12d, r13d
    jge done
    ; j = 0
    xor r14d, r14d
outer_j:
    cmp r14d, r11d
    jge next_i

    xorpd xmm0, xmm0     ; sum = 0.0
    xor r15d, r15d        ; k = 0

inner_k:
    cmp r15d, r10d
    jge store_result

    mov eax, r12d
    imul eax, r10d
    add eax, r15d
    imul rax, 8
    movsd xmm1, qword ptr [rcx + rax]

    mov eax, r15d
    imul eax, r11d
    add eax, r14d
    imul rax, 8
    movsd xmm2, qword ptr [rdx + rax]

    mulsd xmm1, xmm2
    addsd xmm0, xmm1

    inc r15d
    jmp inner_k

store_result:
    mov eax, r12d
    imul eax, r11d
    add eax, r14d
    imul rax, 8
    movsd qword ptr [r8 + rax], xmm0

    inc r14d
    jmp outer_j

next_i:
    inc r12d
    jmp outer_i

done:
    pop r15
    pop r14
    pop rbx
    ret
MultiplyAsmPartial ENDP

END
END

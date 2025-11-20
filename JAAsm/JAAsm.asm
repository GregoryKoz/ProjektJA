PUBLIC MultiplyAsmPartial

.code

; MultiplyAsmPartial(double* A, double* B, double* C,
;                    int rowsA, int colsA, int colsB,
;                    int startRow, int endRow)
;
; RCX = A
; RDX = B
; R8  = C
; R9  = rowsA  (nieu¿ywane)
; [rsp+28h] = colsA   (int)
; [rsp+30h] = colsB   (int, PADDED do wielokrotnoœci 4!)
; [rsp+38h] = startRow (int)
; [rsp+40h] = endRow   (int)

MultiplyAsmPartial PROC

    mov     r10d, dword ptr [rsp+28h]   ; colsA
    mov     r11d, dword ptr [rsp+30h]   ; colsB (padded, %4==0)
    mov     r12d, dword ptr [rsp+38h]   ; startRow
    mov     r13d, dword ptr [rsp+40h]   ; endRow

    push    rbx
    push    r14
    push    r15

outer_i:
    cmp     r12d, r13d          ; i >= endRow ?
    jge     done

    xor     r14d, r14d          ; j = 0

vec_j_loop:
    cmp     r14d, r11d          ; j < colsB ?
    jge     next_i

    vpxor   ymm0, ymm0, ymm0    ; sum[0..3] = 0
    xor     ebx, ebx            ; k = 0

vec_inner_k:
    cmp     ebx, r10d           ; k < colsA ?
    jge     vec_store

    ; A[i*colsA + k]
    mov     eax, r12d
    imul    eax, r10d
    add     eax, ebx
    imul    rax, 8
    vbroadcastsd ymm1, qword ptr [rcx + rax]

    ; B[k*colsB + j .. j+3]
    mov     eax, ebx
    imul    eax, r11d
    add     eax, r14d
    imul    rax, 8
    vmovupd ymm2, ymmword ptr [rdx + rax]

    vmulpd  ymm2, ymm2, ymm1
    vaddpd  ymm0, ymm0, ymm2

    inc     ebx
    jmp     vec_inner_k

vec_store:
    ; C[i*colsB + j .. j+3] = sum
    mov     eax, r12d
    imul    eax, r11d
    add     eax, r14d
    imul    rax, 8
    vmovupd ymmword ptr [r8 + rax], ymm0

    add     r14d, 4             ; j += 4
    jmp     vec_j_loop

next_i:
    inc     r12d
    jmp     outer_i

done:
    pop     r15
    pop     r14
    pop     rbx
    vzeroupper                  ; dla bezpieczeñstwa po AVX
    ret

MultiplyAsmPartial ENDP

END
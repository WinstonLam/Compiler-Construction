gcd:
    iload_0
    iload_1
    irem
    ireturn

fac:
    esr 3
    iloadc_0
    istore 1
    iloadc_1
    istore 2
    iload_0
    istore 3
6_while:
    iload_2
    iload_3
    ilt
    branch_f 7_end
    iload_2
    iload_0
    imul
    istore 1
    iinc_1 2               ; add -> inc
    jump 6_while
7_end:
    iload_1
    ireturn

fib:
    iload_0
    iloadc_1
    ile
    branch_f 4_else
    iload_0
    ireturn
    jump 5_end
4_else:
    isrg
    iload_0
    iloadc_1
    isub
    jsr 1 fib
    isrg
    iload_0
    iloadc 0
    isub
    jsr 1 fib
    iadd
    ireturn
5_end:
    iloadc_0
    ireturn

isprime:
    esr 3
    bloadc_t
    bstore 1
    iloadc 0
    istore 2
    iload_0
    istore 3
1_while:
    iload_2
    iload_3
    ilt
    branch_f 2_end
    iload_0
    iload_2
    irem
    iloadc_0
    ieq
    branch_f 3_end
    bloadc_f
    bstore 1
    bload_1
    breturn
3_end:
    iinc_1 2               ; add -> inc
    jump 1_while
2_end:
    bload_1
    breturn

main:
    isrg
    isrg
    iloadc 1
    jsr 1 fac
    jsre 0
    isrg
    iloadc_1
    jsre 5
    isrg
    isrg
    iloadc 2
    iloadc 3
    jsr 2 gcd
    jsre 0
    isrg
    iloadc_1
    jsre 5
    isrg
    isrg
    iloadc 4
    jsr 1 fib
    jsre 0
    isrg
    iloadc_1
    jsre 5
    isrg
    iloadc 4
    jsr 1 isprime
    bpop
    iloadc_0
    ireturn

.const int 2
.const int 5
.const int 10
.const int 6
.const int 3
.exportfun "main" int main
.importfun "printInt" void int
.importfun "printFloat" void float
.importfun "scanInt" int
.importfun "scanFloat" float
.importfun "printSpaces" void int
.importfun "printNewlines" void int

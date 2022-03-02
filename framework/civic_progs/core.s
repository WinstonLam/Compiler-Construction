gcd:
    esr 4
    iloadc_1
    istore 2
    iloadc_0
    istore 3
    iload_0
    iload_1
    igt
    branch_f 13_else
    iload_1
    istore 3
    jump 14_end
13_else:
    iload_0
    istore 3
14_end:
    iloadc_1
    istore 4
    iload_3
    istore 5
8_while:
    iload 4
    iload 5
    ilt
    branch_f 9_end
    iload_0
    iload 4
    irem
    iloadc_0
    ieq
    branch_f 11_false_expr
    iload_1
    iload 4
    irem
    iloadc_0
    ieq
    jump 12_end
11_false_expr:
    bloadc_f
12_end:
    branch_f 10_end
    iload 4
    istore 2
10_end:
    iinc_1 4               ; add -> inc
    jump 8_while
9_end:
    iload_2
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

.const int 2
.exportfun "gcd" int int int gcd
.exportfun "fac" int int fac
.exportfun "fib" int int fib
.exportfun "isprime" bool int isprime
.importfun "printInt" void int
.importfun "printFloat" void float
.importfun "scanInt" int
.importfun "scanFloat" float
.importfun "printSpaces" void int
.importfun "printNewlines" void int

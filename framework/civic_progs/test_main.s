main:
    esr 5
    iloadc 0
    inewa
    astore 0
    iloadc_1
    iloadc_0
    aload_0
    istorea
    iloadc 1
    iloadc_1
    aload_0
    istorea
    iloadc 0
    iloadc 1
    aload_0
    istorea
    iloadc 0
    fnewa
    astore 1
    floadc_1
    iloadc_0
    aload_1
    fstorea
    floadc 2
    iloadc_1
    aload_1
    fstorea
    floadc 3
    iloadc 1
    aload_1
    fstorea
    iloadc 4
    inewa
    astore 2
    iloadc_1
    iloadc_0
    aload_2
    istorea
    iloadc 1
    iloadc_1
    aload_2
    istorea
    iloadc 0
    iloadc 1
    aload_2
    istorea
    iloadc 4
    iloadc 0
    aload_2
    istorea
    iloadc 4
    fnewa
    astore 3
    floadc_1
    iloadc_0
    aload_3
    fstorea
    floadc 2
    iloadc_1
    aload_3
    fstorea
    floadc 3
    iloadc 1
    aload_3
    fstorea
    floadc 5
    iloadc 0
    aload_3
    fstorea
    iloadc 4
    fnewa
    astore 4
    isrg
    isrg
    iloadc 6
    jsre 6
    jsre 0
    isrg
    iloadc_1
    jsre 5
    isrg
    isrg
    iloadc 7
    iloadc 8
    jsre 7
    jsre 0
    isrg
    iloadc_1
    jsre 5
    iloadc_0
    ireturn

.const int 3
.const int 2
.const float 2.0
.const float 3.0
.const int 4
.const float 4.0
.const int 5
.const int 10
.const int 6
.exportfun "main" int main
.importfun "printInt" void int
.importfun "printFloat" void float
.importfun "scanInt" int
.importfun "scanFloat" float
.importfun "printSpaces" void int
.importfun "printNewlines" void int
.importfun "fac" int int
.importfun "gcd" int int int

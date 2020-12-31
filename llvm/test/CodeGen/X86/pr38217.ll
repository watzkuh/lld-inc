; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=x86_64-unknown-unknown | FileCheck %s

@_ZL11DIGIT_TABLE = dso_local constant [201 x i8] c"00010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899\00", align 16

define dso_local void @_Z12d2s_bufferedmPc(i64, i8* nocapture) {
; CHECK-LABEL: _Z12d2s_bufferedmPc:
; CHECK:       # %bb.0:
; CHECK-NEXT:    cmpq $10000, %rdi # imm = 0x2710
; CHECK-NEXT:    jb .LBB0_3
; CHECK-NEXT:  # %bb.1: # %.preheader
; CHECK-NEXT:    movq %rdi, %r9
; CHECK-NEXT:    xorl %r10d, %r10d
; CHECK-NEXT:    movabsq $3777893186295716171, %r8 # imm = 0x346DC5D63886594B
; CHECK-NEXT:    .p2align 4, 0x90
; CHECK-NEXT:  .LBB0_2: # =>This Inner Loop Header: Depth=1
; CHECK-NEXT:    movq %r9, %rax
; CHECK-NEXT:    mulq %r8
; CHECK-NEXT:    shrq $11, %rdx
; CHECK-NEXT:    imulq $10000, %rdx, %rax # imm = 0x2710
; CHECK-NEXT:    movq %r9, %rdi
; CHECK-NEXT:    subq %rax, %rdi
; CHECK-NEXT:    imulq $1374389535, %rdi, %rax # imm = 0x51EB851F
; CHECK-NEXT:    shrq $37, %rax
; CHECK-NEXT:    imull $100, %eax, %ecx
; CHECK-NEXT:    subl %ecx, %edi
; CHECK-NEXT:    movl %r10d, %r11d
; CHECK-NEXT:    movq %rsi, %rcx
; CHECK-NEXT:    subq %r11, %rcx
; CHECK-NEXT:    movzwl _ZL11DIGIT_TABLE(%rdi,%rdi), %edi
; CHECK-NEXT:    movw %di, -1(%rcx)
; CHECK-NEXT:    movzwl _ZL11DIGIT_TABLE(%rax,%rax), %eax
; CHECK-NEXT:    movw %ax, -3(%rcx)
; CHECK-NEXT:    addl $4, %r10d
; CHECK-NEXT:    cmpq $99999999, %r9 # imm = 0x5F5E0FF
; CHECK-NEXT:    movq %rdx, %r9
; CHECK-NEXT:    ja .LBB0_2
; CHECK-NEXT:  .LBB0_3:
; CHECK-NEXT:    retq
  %3 = icmp ugt i64 %0, 9999
  br i1 %3, label %4, label %31

; <label>:4:                                      ; preds = %2, %4
  %5 = phi i64 [ %9, %4 ], [ %0, %2 ]
  %6 = phi i32 [ %29, %4 ], [ 0, %2 ]
  %7 = urem i64 %5, 10000
  %8 = trunc i64 %7 to i32
  %9 = udiv i64 %5, 10000
  %10 = urem i32 %8, 100
  %11 = shl nuw nsw i32 %10, 1
  %12 = udiv i32 %8, 100
  %13 = shl nuw nsw i32 %12, 1
  %14 = zext i32 %6 to i64
  %15 = sub nsw i64 0, %14
  %16 = getelementptr inbounds i8, i8* %1, i64 %15
  %17 = getelementptr inbounds i8, i8* %16, i64 -1
  %18 = zext i32 %11 to i64
  %19 = getelementptr inbounds [201 x i8], [201 x i8]* @_ZL11DIGIT_TABLE, i64 0, i64 %18
  %20 = bitcast i8* %19 to i16*
  %21 = bitcast i8* %17 to i16*
  %22 = load i16, i16* %20, align 2
  store i16 %22, i16* %21, align 1
  %23 = getelementptr inbounds i8, i8* %16, i64 -3
  %24 = zext i32 %13 to i64
  %25 = getelementptr inbounds [201 x i8], [201 x i8]* @_ZL11DIGIT_TABLE, i64 0, i64 %24
  %26 = bitcast i8* %25 to i16*
  %27 = bitcast i8* %23 to i16*
  %28 = load i16, i16* %26, align 2
  store i16 %28, i16* %27, align 1
  %29 = add i32 %6, 4
  %30 = icmp ugt i64 %5, 99999999
  br i1 %30, label %4, label %31

; <label>:31:                                     ; preds = %4, %2
  ret void
}

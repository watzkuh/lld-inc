; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=thumbv8.1m.main -mattr=+mve -tail-predication=enabled %s -o - | FileCheck %s

define dso_local arm_aapcs_vfpcc i32 @minmaxval4(i32* nocapture readonly %x, i32* nocapture %minp) {
; CHECK-LABEL: minmaxval4:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    push {r7, lr}
; CHECK-NEXT:    vpush {d8, d9, d10, d11}
; CHECK-NEXT:    sub sp, #8
; CHECK-NEXT:    mov.w lr, #3
; CHECK-NEXT:    adr r3, .LCPI0_0
; CHECK-NEXT:    dls lr, lr
; CHECK-NEXT:    vldrw.u32 q2, [r3]
; CHECK-NEXT:    vmov.i32 q0, #0x80000000
; CHECK-NEXT:    vmvn.i32 q1, #0x80000000
; CHECK-NEXT:    movs r2, #0
; CHECK-NEXT:    vmov.i32 q3, #0xa
; CHECK-NEXT:  .LBB0_1: @ %vector.body
; CHECK-NEXT:    @ =>This Inner Loop Header: Depth=1
; CHECK-NEXT:    vadd.i32 q4, q2, r2
; CHECK-NEXT:    vdup.32 q5, r2
; CHECK-NEXT:    vcmp.u32 hi, q5, q4
; CHECK-NEXT:    adds r2, #4
; CHECK-NEXT:    vpnot
; CHECK-NEXT:    vpst
; CHECK-NEXT:    vcmpt.u32 hi, q3, q4
; CHECK-NEXT:    vstr p0, [sp, #4] @ 4-byte Spill
; CHECK-NEXT:    vpst
; CHECK-NEXT:    vldrwt.u32 q4, [r0], #16
; CHECK-NEXT:    vldr p0, [sp, #4] @ 4-byte Reload
; CHECK-NEXT:    vpst
; CHECK-NEXT:    vcmpt.s32 gt, q4, q0
; CHECK-NEXT:    vpsel q0, q4, q0
; CHECK-NEXT:    vldr p0, [sp, #4] @ 4-byte Reload
; CHECK-NEXT:    vpst
; CHECK-NEXT:    vcmpt.s32 gt, q1, q4
; CHECK-NEXT:    vpsel q1, q4, q1
; CHECK-NEXT:    le lr, .LBB0_1
; CHECK-NEXT:  @ %bb.2: @ %middle.block
; CHECK-NEXT:    mvn r0, #-2147483648
; CHECK-NEXT:    vminv.s32 r0, q1
; CHECK-NEXT:    str r0, [r1]
; CHECK-NEXT:    mov.w r0, #-2147483648
; CHECK-NEXT:    vmaxv.s32 r0, q0
; CHECK-NEXT:    add sp, #8
; CHECK-NEXT:    vpop {d8, d9, d10, d11}
; CHECK-NEXT:    pop {r7, pc}
; CHECK-NEXT:    .p2align 4
; CHECK-NEXT:  @ %bb.3:
; CHECK-NEXT:  .LCPI0_0:
; CHECK-NEXT:    .long 0 @ 0x0
; CHECK-NEXT:    .long 1 @ 0x1
; CHECK-NEXT:    .long 2 @ 0x2
; CHECK-NEXT:    .long 3 @ 0x3
entry:
  br label %vector.body

vector.body:                                      ; preds = %vector.body, %entry
  %index = phi i32 [ 0, %entry ], [ %index.next, %vector.body ]
  %vec.phi = phi <4 x i32> [ <i32 2147483647, i32 2147483647, i32 2147483647, i32 2147483647>, %entry ], [ %5, %vector.body ]
  %vec.phi29 = phi <4 x i32> [ <i32 -2147483648, i32 -2147483648, i32 -2147483648, i32 -2147483648>, %entry ], [ %7, %vector.body ]
  %0 = getelementptr inbounds i32, i32* %x, i32 %index
  %active.lane.mask = call <4 x i1> @llvm.get.active.lane.mask.v4i1.i32(i32 %index, i32 10)
  %1 = bitcast i32* %0 to <4 x i32>*
  %wide.masked.load = call <4 x i32> @llvm.masked.load.v4i32.p0v4i32(<4 x i32>* %1, i32 4, <4 x i1> %active.lane.mask, <4 x i32> undef)
  %2 = icmp sgt <4 x i32> %wide.masked.load, %vec.phi29
  %3 = icmp slt <4 x i32> %wide.masked.load, %vec.phi
  %4 = and <4 x i1> %active.lane.mask, %3
  %5 = select <4 x i1> %4, <4 x i32> %wide.masked.load, <4 x i32> %vec.phi
  %6 = and <4 x i1> %active.lane.mask, %2
  %7 = select <4 x i1> %6, <4 x i32> %wide.masked.load, <4 x i32> %vec.phi29
  %index.next = add i32 %index, 4
  %8 = icmp eq i32 %index.next, 12
  br i1 %8, label %middle.block, label %vector.body

middle.block:                                     ; preds = %vector.body
  %9 = call i32 @llvm.vector.reduce.smax.v4i32(<4 x i32> %7)
  %10 = call i32 @llvm.vector.reduce.smin.v4i32(<4 x i32> %5)
  store i32 %10, i32* %minp, align 4
  ret i32 %9
}

declare <4 x i1> @llvm.get.active.lane.mask.v4i1.i32(i32, i32) #1
declare <4 x i32> @llvm.masked.load.v4i32.p0v4i32(<4 x i32>*, i32 immarg, <4 x i1>, <4 x i32>) #2
declare i32 @llvm.vector.reduce.smin.v4i32(<4 x i32>) #3
declare i32 @llvm.vector.reduce.smax.v4i32(<4 x i32>) #3


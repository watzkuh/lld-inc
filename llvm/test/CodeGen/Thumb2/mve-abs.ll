; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=thumbv8.1m.main-none-none-eabi -mattr=+mve -verify-machineinstrs %s -o - | FileCheck %s

define arm_aapcs_vfpcc <16 x i8> @abs_v16i8(<16 x i8> %s1) {
; CHECK-LABEL: abs_v16i8:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vabs.s8 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %0 = icmp slt <16 x i8> %s1, zeroinitializer
  %1 = sub nsw <16 x i8> zeroinitializer, %s1
  %2 = select <16 x i1> %0, <16 x i8> %1, <16 x i8> %s1
  ret <16 x i8> %2
}

define arm_aapcs_vfpcc <8 x i16> @abs_v8i16(<8 x i16> %s1) {
; CHECK-LABEL: abs_v8i16:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vabs.s16 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %0 = icmp slt <8 x i16> %s1, zeroinitializer
  %1 = sub nsw <8 x i16> zeroinitializer, %s1
  %2 = select <8 x i1> %0, <8 x i16> %1, <8 x i16> %s1
  ret <8 x i16> %2
}

define arm_aapcs_vfpcc <4 x i32> @abs_v4i32(<4 x i32> %s1) {
; CHECK-LABEL: abs_v4i32:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vabs.s32 q0, q0
; CHECK-NEXT:    bx lr
entry:
  %0 = icmp slt <4 x i32> %s1, zeroinitializer
  %1 = sub nsw <4 x i32> zeroinitializer, %s1
  %2 = select <4 x i1> %0, <4 x i32> %1, <4 x i32> %s1
  ret <4 x i32> %2
}

define arm_aapcs_vfpcc <2 x i64> @abs_v2i64(<2 x i64> %s1) {
; CHECK-LABEL: abs_v2i64:
; CHECK:       @ %bb.0: @ %entry
; CHECK-NEXT:    vmov r0, s3
; CHECK-NEXT:    vmov r1, s2
; CHECK-NEXT:    vmov r3, s1
; CHECK-NEXT:    vmov r2, s0
; CHECK-NEXT:    adds.w r1, r1, r0, asr #31
; CHECK-NEXT:    adc.w r12, r0, r0, asr #31
; CHECK-NEXT:    eor.w r1, r1, r0, asr #31
; CHECK-NEXT:    adds.w r2, r2, r3, asr #31
; CHECK-NEXT:    eor.w r0, r12, r0, asr #31
; CHECK-NEXT:    eor.w r2, r2, r3, asr #31
; CHECK-NEXT:    vmov q0[2], q0[0], r2, r1
; CHECK-NEXT:    adc.w r1, r3, r3, asr #31
; CHECK-NEXT:    eor.w r1, r1, r3, asr #31
; CHECK-NEXT:    vmov q0[3], q0[1], r1, r0
; CHECK-NEXT:    bx lr
entry:
  %0 = icmp slt <2 x i64> %s1, zeroinitializer
  %1 = sub nsw <2 x i64> zeroinitializer, %s1
  %2 = select <2 x i1> %0, <2 x i64> %1, <2 x i64> %s1
  ret <2 x i64> %2
}

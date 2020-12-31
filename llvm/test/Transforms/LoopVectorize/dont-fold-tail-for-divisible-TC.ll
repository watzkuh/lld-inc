; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -loop-vectorize -force-vector-width=4 -S | FileCheck %s

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"

; Make sure the loop is vectorized under -Os without folding its tail based on
; its trip-count's lower bits known to be zero.

define dso_local void @alignTC(i32* noalias nocapture %A, i32 %n) optsize {
; CHECK-LABEL: @alignTC(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[ALIGNEDTC:%.*]] = and i32 [[N:%.*]], -8
; CHECK-NEXT:    [[MIN_ITERS_CHECK:%.*]] = icmp ult i32 [[ALIGNEDTC]], 4
; CHECK-NEXT:    br i1 [[MIN_ITERS_CHECK]], label [[SCALAR_PH:%.*]], label [[VECTOR_PH:%.*]]
; CHECK:       vector.ph:
; CHECK-NEXT:    [[N_MOD_VF:%.*]] = urem i32 [[ALIGNEDTC]], 4
; CHECK-NEXT:    [[N_VEC:%.*]] = sub i32 [[ALIGNEDTC]], [[N_MOD_VF]]
; CHECK-NEXT:    br label [[VECTOR_BODY:%.*]]
; CHECK:       vector.body:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i32 [ 0, [[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], [[VECTOR_BODY]] ]
; CHECK-NEXT:    [[BROADCAST_SPLATINSERT:%.*]] = insertelement <4 x i32> poison, i32 [[INDEX]], i32 0
; CHECK-NEXT:    [[BROADCAST_SPLAT:%.*]] = shufflevector <4 x i32> [[BROADCAST_SPLATINSERT]], <4 x i32> poison, <4 x i32> zeroinitializer
; CHECK-NEXT:    [[INDUCTION:%.*]] = add <4 x i32> [[BROADCAST_SPLAT]], <i32 0, i32 1, i32 2, i32 3>
; CHECK-NEXT:    [[TMP0:%.*]] = add i32 [[INDEX]], 0
; CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds i32, i32* [[A:%.*]], i32 [[TMP0]]
; CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds i32, i32* [[TMP1]], i32 0
; CHECK-NEXT:    [[TMP3:%.*]] = bitcast i32* [[TMP2]] to <4 x i32>*
; CHECK-NEXT:    store <4 x i32> <i32 13, i32 13, i32 13, i32 13>, <4 x i32>* [[TMP3]], align 1
; CHECK-NEXT:    [[INDEX_NEXT]] = add i32 [[INDEX]], 4
; CHECK-NEXT:    [[TMP4:%.*]] = icmp eq i32 [[INDEX_NEXT]], [[N_VEC]]
; CHECK-NEXT:    br i1 [[TMP4]], label [[MIDDLE_BLOCK:%.*]], label [[VECTOR_BODY]], [[LOOP0:!llvm.loop !.*]]
; CHECK:       middle.block:
; CHECK-NEXT:    [[CMP_N:%.*]] = icmp eq i32 [[ALIGNEDTC]], [[N_VEC]]
; CHECK-NEXT:    br i1 [[CMP_N]], label [[EXIT:%.*]], label [[SCALAR_PH]]
; CHECK:       scalar.ph:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i32 [ [[N_VEC]], [[MIDDLE_BLOCK]] ], [ 0, [[ENTRY:%.*]] ]
; CHECK-NEXT:    br label [[LOOP:%.*]]
; CHECK:       loop:
; CHECK-NEXT:    [[RIV:%.*]] = phi i32 [ [[BC_RESUME_VAL]], [[SCALAR_PH]] ], [ [[RIVPLUS1:%.*]], [[LOOP]] ]
; CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr inbounds i32, i32* [[A]], i32 [[RIV]]
; CHECK-NEXT:    store i32 13, i32* [[ARRAYIDX]], align 1
; CHECK-NEXT:    [[RIVPLUS1]] = add nuw nsw i32 [[RIV]], 1
; CHECK-NEXT:    [[COND:%.*]] = icmp eq i32 [[RIVPLUS1]], [[ALIGNEDTC]]
; CHECK-NEXT:    br i1 [[COND]], label [[EXIT]], label [[LOOP]], [[LOOP2:!llvm.loop !.*]]
; CHECK:       exit:
; CHECK-NEXT:    ret void
;
entry:
  %alignedTC = and i32 %n, -8
  br label %loop

loop:
  %riv = phi i32 [ 0, %entry ], [ %rivPlus1, %loop ]
  %arrayidx = getelementptr inbounds i32, i32* %A, i32 %riv
  store i32 13, i32* %arrayidx, align 1
  %rivPlus1 = add nuw nsw i32 %riv, 1
  %cond = icmp eq i32 %rivPlus1, %alignedTC
  br i1 %cond, label %exit, label %loop

exit:
  ret void
}

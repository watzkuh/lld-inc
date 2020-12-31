; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -force-vector-width=2 -force-vector-interleave=1 -loop-vectorize -S | FileCheck %s

; Test case for PR44488. Checks that the correct predicates are created for
; branches where true and false successors are equal. See the checks involving
; CMP1 and CMP2.

@v_38 = global i16 12061, align 1
@v_39 = global i16 11333, align 1

define i16 @test_true_and_false_branch_equal() {
; CHECK-LABEL: @test_true_and_false_branch_equal(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    br i1 false, label [[SCALAR_PH:%.*]], label [[VECTOR_PH:%.*]]
; CHECK:       vector.ph:
; CHECK-NEXT:    br label [[VECTOR_BODY:%.*]]
; CHECK:       vector.body:
; CHECK-NEXT:    [[INDEX:%.*]] = phi i32 [ 0, [[VECTOR_PH]] ], [ [[INDEX_NEXT:%.*]], [[PRED_SREM_CONTINUE4:%.*]] ]
; CHECK-NEXT:    [[TMP0:%.*]] = trunc i32 [[INDEX]] to i16
; CHECK-NEXT:    [[OFFSET_IDX:%.*]] = add i16 99, [[TMP0]]
; CHECK-NEXT:    [[BROADCAST_SPLATINSERT:%.*]] = insertelement <2 x i16> poison, i16 [[OFFSET_IDX]], i32 0
; CHECK-NEXT:    [[BROADCAST_SPLAT:%.*]] = shufflevector <2 x i16> [[BROADCAST_SPLATINSERT]], <2 x i16> poison, <2 x i32> zeroinitializer
; CHECK-NEXT:    [[INDUCTION:%.*]] = add <2 x i16> [[BROADCAST_SPLAT]], <i16 0, i16 1>
; CHECK-NEXT:    [[TMP1:%.*]] = add i16 [[OFFSET_IDX]], 0
; CHECK-NEXT:    [[TMP2:%.*]] = load i16, i16* @v_38, align 1
; CHECK-NEXT:    [[BROADCAST_SPLATINSERT1:%.*]] = insertelement <2 x i16> poison, i16 [[TMP2]], i32 0
; CHECK-NEXT:    [[BROADCAST_SPLAT2:%.*]] = shufflevector <2 x i16> [[BROADCAST_SPLATINSERT1]], <2 x i16> poison, <2 x i32> zeroinitializer
; CHECK-NEXT:    [[TMP3:%.*]] = icmp eq <2 x i16> [[BROADCAST_SPLAT2]], <i16 32767, i16 32767>
; CHECK-NEXT:    [[TMP4:%.*]] = icmp eq <2 x i16> [[BROADCAST_SPLAT2]], zeroinitializer
; CHECK-NEXT:    [[TMP5:%.*]] = xor <2 x i1> [[TMP4]], <i1 true, i1 true>
; CHECK-NEXT:    [[TMP6:%.*]] = extractelement <2 x i1> [[TMP5]], i32 0
; CHECK-NEXT:    br i1 [[TMP6]], label [[PRED_SREM_IF:%.*]], label [[PRED_SREM_CONTINUE:%.*]]
; CHECK:       pred.srem.if:
; CHECK-NEXT:    [[TMP7:%.*]] = srem i16 5786, [[TMP2]]
; CHECK-NEXT:    [[TMP8:%.*]] = insertelement <2 x i16> undef, i16 [[TMP7]], i32 0
; CHECK-NEXT:    br label [[PRED_SREM_CONTINUE]]
; CHECK:       pred.srem.continue:
; CHECK-NEXT:    [[TMP9:%.*]] = phi <2 x i16> [ undef, [[VECTOR_BODY]] ], [ [[TMP8]], [[PRED_SREM_IF]] ]
; CHECK-NEXT:    [[TMP10:%.*]] = extractelement <2 x i1> [[TMP5]], i32 1
; CHECK-NEXT:    br i1 [[TMP10]], label [[PRED_SREM_IF3:%.*]], label [[PRED_SREM_CONTINUE4]]
; CHECK:       pred.srem.if3:
; CHECK-NEXT:    [[TMP11:%.*]] = srem i16 5786, [[TMP2]]
; CHECK-NEXT:    [[TMP12:%.*]] = insertelement <2 x i16> [[TMP9]], i16 [[TMP11]], i32 1
; CHECK-NEXT:    br label [[PRED_SREM_CONTINUE4]]
; CHECK:       pred.srem.continue4:
; CHECK-NEXT:    [[TMP13:%.*]] = phi <2 x i16> [ [[TMP9]], [[PRED_SREM_CONTINUE]] ], [ [[TMP12]], [[PRED_SREM_IF3]] ]
; CHECK-NEXT:    [[PREDPHI:%.*]] = select <2 x i1> [[TMP4]], <2 x i16> <i16 5786, i16 5786>, <2 x i16> [[TMP13]]
; CHECK-NEXT:    [[TMP14:%.*]] = extractelement <2 x i16> [[PREDPHI]], i32 0
; CHECK-NEXT:    store i16 [[TMP14]], i16* @v_39, align 1
; CHECK-NEXT:    [[TMP15:%.*]] = extractelement <2 x i16> [[PREDPHI]], i32 1
; CHECK-NEXT:    store i16 [[TMP15]], i16* @v_39, align 1
; CHECK-NEXT:    [[INDEX_NEXT]] = add i32 [[INDEX]], 2
; CHECK-NEXT:    [[TMP16:%.*]] = icmp eq i32 [[INDEX_NEXT]], 12
; CHECK-NEXT:    br i1 [[TMP16]], label [[MIDDLE_BLOCK:%.*]], label [[VECTOR_BODY]], [[LOOP0:!llvm.loop !.*]]
; CHECK:       middle.block:
; CHECK-NEXT:    [[CMP_N:%.*]] = icmp eq i32 12, 12
; CHECK-NEXT:    br i1 [[CMP_N]], label [[EXIT:%.*]], label [[SCALAR_PH]]
; CHECK:       scalar.ph:
; CHECK-NEXT:    [[BC_RESUME_VAL:%.*]] = phi i16 [ 111, [[MIDDLE_BLOCK]] ], [ 99, [[ENTRY:%.*]] ]
; CHECK-NEXT:    br label [[FOR_BODY:%.*]]
; CHECK:       for.body:
; CHECK-NEXT:    [[I_07:%.*]] = phi i16 [ [[BC_RESUME_VAL]], [[SCALAR_PH]] ], [ [[INC7:%.*]], [[FOR_LATCH:%.*]] ]
; CHECK-NEXT:    [[LV:%.*]] = load i16, i16* @v_38, align 1
; CHECK-NEXT:    [[CMP1:%.*]] = icmp eq i16 [[LV]], 32767
; CHECK-NEXT:    br i1 [[CMP1]], label [[COND_END:%.*]], label [[COND_END]]
; CHECK:       cond.end:
; CHECK-NEXT:    [[CMP2:%.*]] = icmp eq i16 [[LV]], 0
; CHECK-NEXT:    br i1 [[CMP2]], label [[FOR_LATCH]], label [[COND_FALSE4:%.*]]
; CHECK:       cond.false4:
; CHECK-NEXT:    [[REM:%.*]] = srem i16 5786, [[LV]]
; CHECK-NEXT:    br label [[FOR_LATCH]]
; CHECK:       for.latch:
; CHECK-NEXT:    [[COND6:%.*]] = phi i16 [ [[REM]], [[COND_FALSE4]] ], [ 5786, [[COND_END]] ]
; CHECK-NEXT:    store i16 [[COND6]], i16* @v_39, align 1
; CHECK-NEXT:    [[INC7]] = add nsw i16 [[I_07]], 1
; CHECK-NEXT:    [[CMP:%.*]] = icmp slt i16 [[INC7]], 111
; CHECK-NEXT:    br i1 [[CMP]], label [[FOR_BODY]], label [[EXIT]], [[LOOP2:!llvm.loop !.*]]
; CHECK:       exit:
; CHECK-NEXT:    [[RV:%.*]] = load i16, i16* @v_39, align 1
; CHECK-NEXT:    ret i16 [[RV]]
;
entry:
  br label %for.body

for.body:                                         ; preds = %entry, %for.latch
  %i.07 = phi i16 [ 99, %entry ], [ %inc7, %for.latch ]
  %lv = load i16, i16* @v_38, align 1
  %cmp1 = icmp eq i16 %lv, 32767
  br i1 %cmp1, label %cond.end, label %cond.end

cond.end:                                         ; preds = %for.body, %for.body
  %cmp2 = icmp eq i16 %lv, 0
  br i1 %cmp2, label %for.latch, label %cond.false4

cond.false4:                                      ; preds = %cond.end
  %rem = srem i16 5786, %lv
  br label %for.latch

for.latch:                                        ; preds = %cond.end, %cond.false4
  %cond6 = phi i16 [ %rem, %cond.false4 ], [ 5786, %cond.end ]
  store i16 %cond6, i16* @v_39, align 1
  %inc7 = add nsw i16 %i.07, 1
  %cmp = icmp slt i16 %inc7, 111
  br i1 %cmp, label %for.body, label %exit

exit:                                 ; preds = %for.latch
  %rv = load i16, i16* @v_39, align 1
  ret i16 %rv
}

; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -mtriple=amdgcn-amd-amdhsa -mcpu=gfx600 -verify-machineinstrs < %s | FileCheck --check-prefixes=GFX6 %s
; RUN: llc -mtriple=amdgcn-amd-amdhsa -mcpu=gfx700 -verify-machineinstrs < %s | FileCheck --check-prefixes=GFX7 %s
; RUN: llc -mtriple=amdgcn-amd-amdhsa -mcpu=gfx1010 -verify-machineinstrs < %s | FileCheck --check-prefixes=GFX10-WGP %s
; RUN: llc -mtriple=amdgcn-amd-amdhsa -mcpu=gfx1010 -mattr=+cumode -verify-machineinstrs < %s | FileCheck --check-prefixes=GFX10-CU %s
; RUN: llc -mtriple=amdgcn-amd-amdpal -mcpu=gfx700 -amdgcn-skip-cache-invalidations -verify-machineinstrs < %s | FileCheck --check-prefixes=SKIP-CACHE-INV %s

define amdgpu_kernel void @singlethread_acquire_fence() {
; GFX6-LABEL: singlethread_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread") acquire
  ret void
}

define amdgpu_kernel void @singlethread_release_fence() {
; GFX6-LABEL: singlethread_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread") release
  ret void
}

define amdgpu_kernel void @singlethread_acq_rel_fence() {
; GFX6-LABEL: singlethread_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread") acq_rel
  ret void
}

define amdgpu_kernel void @singlethread_seq_cst_fence() {
; GFX6-LABEL: singlethread_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread") seq_cst
  ret void
}

define amdgpu_kernel void @singlethread_one_as_acquire_fence() {
; GFX6-LABEL: singlethread_one_as_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_one_as_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_one_as_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_one_as_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_one_as_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread-one-as") acquire
  ret void
}

define amdgpu_kernel void @singlethread_one_as_release_fence() {
; GFX6-LABEL: singlethread_one_as_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_one_as_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_one_as_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_one_as_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_one_as_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread-one-as") release
  ret void
}

define amdgpu_kernel void @singlethread_one_as_acq_rel_fence() {
; GFX6-LABEL: singlethread_one_as_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_one_as_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_one_as_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_one_as_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_one_as_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread-one-as") acq_rel
  ret void
}

define amdgpu_kernel void @singlethread_one_as_seq_cst_fence() {
; GFX6-LABEL: singlethread_one_as_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: singlethread_one_as_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: singlethread_one_as_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: singlethread_one_as_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: singlethread_one_as_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("singlethread-one-as") seq_cst
  ret void
}

define amdgpu_kernel void @wavefront_acquire_fence() {
; GFX6-LABEL: wavefront_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront") acquire
  ret void
}

define amdgpu_kernel void @wavefront_release_fence() {
; GFX6-LABEL: wavefront_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront") release
  ret void
}

define amdgpu_kernel void @wavefront_acq_rel_fence() {
; GFX6-LABEL: wavefront_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront") acq_rel
  ret void
}

define amdgpu_kernel void @wavefront_seq_cst_fence() {
; GFX6-LABEL: wavefront_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront") seq_cst
  ret void
}

define amdgpu_kernel void @wavefront_one_as_acquire_fence() {
; GFX6-LABEL: wavefront_one_as_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_one_as_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_one_as_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_one_as_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_one_as_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront-one-as") acquire
  ret void
}

define amdgpu_kernel void @wavefront_one_as_release_fence() {
; GFX6-LABEL: wavefront_one_as_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_one_as_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_one_as_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_one_as_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_one_as_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront-one-as") release
  ret void
}

define amdgpu_kernel void @wavefront_one_as_acq_rel_fence() {
; GFX6-LABEL: wavefront_one_as_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_one_as_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_one_as_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_one_as_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_one_as_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront-one-as") acq_rel
  ret void
}

define amdgpu_kernel void @wavefront_one_as_seq_cst_fence() {
; GFX6-LABEL: wavefront_one_as_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: wavefront_one_as_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: wavefront_one_as_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: wavefront_one_as_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: wavefront_one_as_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("wavefront-one-as") seq_cst
  ret void
}

define amdgpu_kernel void @workgroup_acquire_fence() {
; GFX6-LABEL: workgroup_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt lgkmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt lgkmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt lgkmcnt(0)
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup") acquire
  ret void
}

define amdgpu_kernel void @workgroup_release_fence() {
; GFX6-LABEL: workgroup_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt lgkmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt lgkmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt lgkmcnt(0)
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup") release
  ret void
}

define amdgpu_kernel void @workgroup_acq_rel_fence() {
; GFX6-LABEL: workgroup_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt lgkmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt lgkmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt lgkmcnt(0)
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup") acq_rel
  ret void
}

define amdgpu_kernel void @workgroup_seq_cst_fence() {
; GFX6-LABEL: workgroup_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt lgkmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt lgkmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt lgkmcnt(0)
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup") seq_cst
  ret void
}

define amdgpu_kernel void @workgroup_one_as_acquire_fence() {
; GFX6-LABEL: workgroup_one_as_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_one_as_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_one_as_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_one_as_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_one_as_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup-one-as") acquire
  ret void
}

define amdgpu_kernel void @workgroup_one_as_release_fence() {
; GFX6-LABEL: workgroup_one_as_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_one_as_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_one_as_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_one_as_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_one_as_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup-one-as") release
  ret void
}

define amdgpu_kernel void @workgroup_one_as_acq_rel_fence() {
; GFX6-LABEL: workgroup_one_as_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_one_as_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_one_as_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_one_as_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_one_as_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup-one-as") acq_rel
  ret void
}

define amdgpu_kernel void @workgroup_one_as_seq_cst_fence() {
; GFX6-LABEL: workgroup_one_as_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: workgroup_one_as_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: workgroup_one_as_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: workgroup_one_as_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: workgroup_one_as_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("workgroup-one-as") seq_cst
  ret void
}

define amdgpu_kernel void @agent_acquire_fence() {
; GFX6-LABEL: agent_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent") acquire
  ret void
}

define amdgpu_kernel void @agent_release_fence() {
; GFX6-LABEL: agent_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent") release
  ret void
}

define amdgpu_kernel void @agent_acq_rel_fence() {
; GFX6-LABEL: agent_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent") acq_rel
  ret void
}

define amdgpu_kernel void @agent_seq_cst_fence() {
; GFX6-LABEL: agent_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent") seq_cst
  ret void
}

define amdgpu_kernel void @agent_one_as_acquire_fence() {
; GFX6-LABEL: agent_one_as_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_one_as_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_one_as_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_one_as_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_one_as_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent-one-as") acquire
  ret void
}

define amdgpu_kernel void @agent_one_as_release_fence() {
; GFX6-LABEL: agent_one_as_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_one_as_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_one_as_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_one_as_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_one_as_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent-one-as") release
  ret void
}

define amdgpu_kernel void @agent_one_as_acq_rel_fence() {
; GFX6-LABEL: agent_one_as_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_one_as_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_one_as_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_one_as_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_one_as_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent-one-as") acq_rel
  ret void
}

define amdgpu_kernel void @agent_one_as_seq_cst_fence() {
; GFX6-LABEL: agent_one_as_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: agent_one_as_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: agent_one_as_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: agent_one_as_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: agent_one_as_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("agent-one-as") seq_cst
  ret void
}

define amdgpu_kernel void @system_acquire_fence() {
; GFX6-LABEL: system_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence acquire
  ret void
}

define amdgpu_kernel void @system_release_fence() {
; GFX6-LABEL: system_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence release
  ret void
}

define amdgpu_kernel void @system_acq_rel_fence() {
; GFX6-LABEL: system_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence acq_rel
  ret void
}

define amdgpu_kernel void @system_seq_cst_fence() {
; GFX6-LABEL: system_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0) lgkmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence seq_cst
  ret void
}

define amdgpu_kernel void @system_one_as_acquire_fence() {
; GFX6-LABEL: system_one_as_acquire_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_one_as_acquire_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_one_as_acquire_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_one_as_acquire_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_one_as_acquire_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("one-as") acquire
  ret void
}

define amdgpu_kernel void @system_one_as_release_fence() {
; GFX6-LABEL: system_one_as_release_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_one_as_release_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_one_as_release_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_one_as_release_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_one_as_release_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("one-as") release
  ret void
}

define amdgpu_kernel void @system_one_as_acq_rel_fence() {
; GFX6-LABEL: system_one_as_acq_rel_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_one_as_acq_rel_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_one_as_acq_rel_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_one_as_acq_rel_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_one_as_acq_rel_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("one-as") acq_rel
  ret void
}

define amdgpu_kernel void @system_one_as_seq_cst_fence() {
; GFX6-LABEL: system_one_as_seq_cst_fence:
; GFX6:       ; %bb.0: ; %entry
; GFX6-NEXT:    s_waitcnt vmcnt(0)
; GFX6-NEXT:    buffer_wbinvl1
; GFX6-NEXT:    s_endpgm
;
; GFX7-LABEL: system_one_as_seq_cst_fence:
; GFX7:       ; %bb.0: ; %entry
; GFX7-NEXT:    s_waitcnt vmcnt(0)
; GFX7-NEXT:    buffer_wbinvl1_vol
; GFX7-NEXT:    s_endpgm
;
; GFX10-WGP-LABEL: system_one_as_seq_cst_fence:
; GFX10-WGP:       ; %bb.0: ; %entry
; GFX10-WGP-NEXT:    s_waitcnt vmcnt(0)
; GFX10-WGP-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-WGP-NEXT:    buffer_gl0_inv
; GFX10-WGP-NEXT:    buffer_gl1_inv
; GFX10-WGP-NEXT:    s_endpgm
;
; GFX10-CU-LABEL: system_one_as_seq_cst_fence:
; GFX10-CU:       ; %bb.0: ; %entry
; GFX10-CU-NEXT:    s_waitcnt vmcnt(0)
; GFX10-CU-NEXT:    s_waitcnt_vscnt null, 0x0
; GFX10-CU-NEXT:    buffer_gl0_inv
; GFX10-CU-NEXT:    buffer_gl1_inv
; GFX10-CU-NEXT:    s_endpgm
;
; SKIP-CACHE-INV-LABEL: system_one_as_seq_cst_fence:
; SKIP-CACHE-INV:       ; %bb.0: ; %entry
; SKIP-CACHE-INV-NEXT:    s_waitcnt vmcnt(0)
; SKIP-CACHE-INV-NEXT:    s_endpgm
entry:
  fence syncscope("one-as") seq_cst
  ret void
}


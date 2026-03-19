; RUN: opt -passes=simple-inliner -S < %s | FileCheck %s

define void @dummy_callee() {
entry:
  call void @do_something()
  ret void
}

declare void @do_something()

define void @caller() {
; CHECK-LABEL: define void @caller()
; CHECK-NEXT: entry:
; CHECK-NEXT:   br label %entrycloned

; CHECK: post.call:
; CHECK-NEXT:   ret void

; CHECK: entrycloned:
; CHECK-NEXT:   call void @do_something()
; CHECK-NEXT:   br label %post.call

entry:
  call void @dummy_callee()
  ret void
}
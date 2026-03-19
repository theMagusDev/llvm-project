; RUN: opt -passes=simple-inliner -S < %s | FileCheck %s

declare void @external_func()

define void @caller() {
; CHECK-LABEL: define void @caller
; CHECK: entry:
; CHECK-NEXT: call void @external_func()
; CHECK-NEXT: ret void
entry:
  call void @external_func()
  ret void
}
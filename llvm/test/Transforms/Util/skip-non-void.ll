; RUN: opt -passes=simple-inliner -S < %s | FileCheck %s

define i32 @callee_returns_int() {
entry:
  ret i32 42
}

define void @caller() {
; CHECK-LABEL: define void @caller
; CHECK: entry:
; CHECK-NEXT: %res = call i32 @callee_returns_int()
; CHECK-NEXT: ret void
entry:
  %res = call i32 @callee_returns_int()
  ret void
}
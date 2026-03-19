// RUN: %clang_cc1 -std=c++17 -load %plugin_path -add-plugin annotate-unused-vars -ast-dump %s | FileCheck %s

int global_var = 100;

void scope_test() {
    static int static_unused = 200;
    int used_in_sizeof = 300;
    unsigned long size_result = sizeof(used_in_sizeof);
}

// CHECK: VarDecl{{.*}} global_var 'int'
// CHECK-NOT: UnusedAttr

// CHECK: FunctionDecl{{.*}} scope_test

// CHECK: VarDecl{{.*}} static_unused 'int'
// CHECK: UnusedAttr

// CHECK: VarDecl{{.*}} used_in_sizeof 'int'
// CHECK-NOT: UnusedAttr

// CHECK: VarDecl{{.*}} size_result 'unsigned long'
// CHECK: UnusedAttr
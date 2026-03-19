// RUN: %clang_cc1 -std=c++17 -load %plugin_path -add-plugin annotate-unused-vars -ast-dump %s | FileCheck %s

void lambda_test() {
    int captured_var = 10;
    
    auto my_lambda = [captured_var](int lambda_param) {
        int inner_unused = captured_var + 1;
    };
}

// CHECK: FunctionDecl{{.*}} lambda_test

// CHECK: VarDecl{{.*}} captured_var 'int'
// CHECK-NOT: UnusedAttr

// CHECK: VarDecl{{.*}} my_lambda

// CHECK: ParmVarDecl{{.*}} lambda_param 'int'
// CHECK: UnusedAttr

// CHECK: VarDecl{{.*}} inner_unused 'int'
// CHECK: UnusedAttr

// CHECK: UnusedAttr
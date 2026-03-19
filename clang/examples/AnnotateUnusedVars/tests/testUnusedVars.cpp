// RUN: %clang_cc1 -std=c++17 -load %plugin_path -add-plugin annotate-unused-vars -ast-dump %s | FileCheck %s

int foo(int a, int b, int c) {
    int used_local = a + b;
    int unused_local = 42;
    return used_local;
  }
  
  // CHECK: FunctionDecl {{.*}} foo
  // CHECK: ParmVarDecl{{.*}} a 'int'
  // CHECK-NOT: UnusedAttr
  // CHECK: ParmVarDecl{{.*}} b 'int'
  // CHECK-NOT: UnusedAttr
  // CHECK: ParmVarDecl{{.*}} c 'int'
  // CHECK-NEXT: UnusedAttr
  
  // CHECK: VarDecl{{.*}} used_local 'int'
  // CHECK-NOT: UnusedAttr
  // CHECK: VarDecl{{.*}} unused_local 'int'
  // CHECK: UnusedAttr
  
  // Проверка, что уже помеченные переменные не трогаем.
  int bar([[maybe_unused]] int x, int y) {
    [[maybe_unused]] int already_marked = 0;
    int should_be_marked = 0;
    (void)y;
    return 0;
  }
  
  // CHECK: FunctionDecl {{.*}} bar
  // CHECK: ParmVarDecl{{.*}} x 'int'
  // CHECK-NEXT: UnusedAttr
  // CHECK: ParmVarDecl{{.*}} y 'int'
  // CHECK-NOT: UnusedAttr
  //
  // CHECK: VarDecl{{.*}} already_marked 'int'
  // CHECK: UnusedAttr
  // CHECK: VarDecl{{.*}} should_be_marked 'int'
  // CHECK: UnusedAttr